#include <locale.h>
#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "bsu.h"

#define imin(x,y) ((x)<(y))?(x):(y)
#define imax(x,y) ((x)>(y))?(x):(y)

// ---------------------------------------------------------------------------------------

extern char _binary_simulation_bsu_start[];
extern char _binary_simulation_bsu_size[];

void bsu_simulation_write_count_age_location_velocity( uintptr_t bsu_start );
int  bsu_draw( uintptr_t bsu_start, int u, int v );

#define kBsuInitialDirectionDown 0
#define kBsuInitialDirectionHero 1
#define kBsuInitialDirectionNone 2
#define kBsuInitialDirectionAlongOffset 3

// ---------------------------------------------------------------------------------------

typedef struct timespec timespec;

int offset_x = 0;
int offset_y = 0;

int debug_a = 0;
int debug_b = 0;
int debug_c = 0;
int debug_d = 0;

int sdCircle( int x, int y, int r )
{
  return sqrt((x*x)+(y*y)) - r;
}

float clampf( float a, float low, float high )
{
  return fmax(fmin(a,high),low);
}

float signf( float a )
{
  return (a<0)?-1.0f:1.0f;
}

float sdPentagon( float px, float py, float r )
{
  float kx = 0.809016994f;
  float ky = 0.587785252f;
  float kz = 0.726542528f;

  px  = fabs(px);

  float d0   = 2.0f * fminf( (px*-kx)+(py*ky) , 0.0f );
  float k1x  = d0 * -kx;
  float k1y  = d0 * ky;

  px -= k1x;
  py -= k1y;

  float d1   = 2.0f * fminf( (px*kx)+(py*ky) , 0.0f );
  float k3x  = d1 * kx;
  float k3y  = d1 * ky;

  px -= k3x;
  py -= k3y;

  float dx   = clampf(px,-r*kz,r*kz);
  float dy   = r;

  px -= dx;
  py -= dy;

  return sqrtf( (px*px)+(py*py) ) * signf(py);
}

int opOnion( int d, int r )
{
  return abs(d)-r;
}

timespec timespec_sub(timespec start, timespec end)
{
	timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

timespec timespec_add(timespec start, timespec end)
{
  int64_t sec  = start.tv_sec + end.tv_sec;
  int64_t nsec = start.tv_nsec + end.tv_nsec;
  int64_t extra_sec  = (nsec/1000000000);
  int64_t extra_nsec = extra_sec * 1000000000;
  timespec sum = { sec+extra_sec, nsec-extra_nsec};
  return sum;
}

void mvprint_timespec( int y, int x, const char* title, timespec time )
{
  int64_t sec  = time.tv_sec;
  int64_t msec = time.tv_nsec / 1000000;
  int64_t usec = (time.tv_nsec - (msec*1000000)) / 10000;
  int64_t nsec = time.tv_nsec - (msec*1000000) - (usec*10000);
  mvprintw(y, x, "%s%ds %dms %dus %dns",title,(int)(sec),(int)(msec),(int)(usec),(int)(nsec));
}

float timespec_f( timespec time )
{
  return ((float) time.tv_sec + (time.tv_nsec / 1000000000.0f));
}

// ---------------------------------------------------------------------------------------

int   iWidth        = 0;
int   iHeight       = 0;
int   iHalfWidth    = 0;
int   iHalfHeight   = 0;
int   iFrameCounter = 0;
float iTimeDelta    = 0.0f;
float iTime         = 0.0f;

int   mouse_x = 0;
int   mouse_y = 0;
int   mouse_bstate = 0;

int mono_fragment( int x, int y );

int main(void)
{
  setlocale(LC_ALL, "");
  
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  set_escdelay(0);

  // Don't mask any mouse events
  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  mouseinterval(0);
  printf("\033[?1003h\n"); // Makes the terminal report mouse movement events

  clear();

  iWidth      = COLS * 2;
  iHeight     = LINES * 4;
  iHalfWidth  = iWidth / 2;
  iHalfHeight = iHeight / 2;

  timespec frame_time;
  timespec sleep_time;
  timespec remaining_time;
  timespec target_time = { 0, 16666666 }; 

  uintptr_t bsu_start = (uintptr_t)_binary_simulation_bsu_start;

  while (1)
  {
    timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    int event_key = 0;
    do 
    {
      event_key = getch();
      switch (event_key)
      {
        case KEY_LEFT:
          offset_x -= 10;
        break;
        case KEY_RIGHT:
          offset_x += 10;
        break;
        case KEY_UP:
          offset_y -= 10;
        break;
        case KEY_DOWN:
          offset_y += 10;
        break;
        case 'q':
          goto quit;
        break;
        case KEY_MOUSE:
        {
          MEVENT event;
          if (getmouse(&event) == OK) 
          {
            mouse_x      = event.x;
            mouse_y      = event.y;
            mouse_bstate = event.bstate;
    
            offset_x = mouse_x * 2;
            offset_y = mouse_y * 4;
          }
        }
        break;
        case KEY_RESIZE:
        { 
          iWidth      = COLS * 2;
          iHeight     = LINES * 4;
          iHalfWidth  = iWidth / 2;
          iHalfHeight = iHeight / 2;
        }
        break;
      }
    } while (event_key != ERR);

    for (int y=0;y<iHeight;y+=4)
    {
      for (int x=0;x<iWidth;x+=2)
      {
        int ch = 0;
  
        ch |= mono_fragment(x+0,y+0)  ?  1 : 0;
        ch |= mono_fragment(x+1,y+0) ?   8 : 0;
        ch |= mono_fragment(x+0,y+1) ?   2 : 0;
        ch |= mono_fragment(x+1,y+1) ?  16 : 0;
        ch |= mono_fragment(x+0,y+2) ?   4 : 0;
        ch |= mono_fragment(x+1,y+2) ?  32 : 0;
        ch |= mono_fragment(x+0,y+3) ?  64 : 0;
        ch |= mono_fragment(x+1,y+3) ? 128 : 0;
  
        mvprintw(y/4,x/2,"%lc",L'\u2800'+ch);
      }
    }


    int line = 0;
    mvprint_timespec( line++,0, "frame_time:      ", frame_time);
    mvprint_timespec( line++,0, "sleep_time:      ", sleep_time);
    mvprintw(line++,0,          "iTime:           %f",iTime);
    mvprintw(line++,0,          "iTimeDelta:      %f",iTimeDelta);
    mvprintw(line++,0,          "iFrameCounter:   %d",iFrameCounter);
    mvprintw(line++, 0,         "iResolution:     w:%d h:%d",iWidth,iHeight);

    mvprintw(line++,0,          "magic:           %s", (char*)bsu_start);

    uintptr_t size_ptr = (uintptr_t)_binary_simulation_bsu_size;
    uint32_t  size     = (uint32_t)size_ptr;
    mvprintw(line++,0,          "size:            0x%08x", size);


    uintptr_t     asset_names_offset_addr = bsu_start + kAssetNamesOffset;
    uint32_t      asset_names_offset      = *(uint32_t*)asset_names_offset_addr;
    uintptr_t     asset_names_start       = bsu_start + asset_names_offset;
    static_array* asset_names_array       = (static_array*)asset_names_start;
    uint32_t      asset_count             = asset_names_array->count;
    mvprintw(line++,0,          "asset_count:     %d", asset_count);
    mvprintw(line++,0,          "mouse            x:%d y:%d bstate:%d", mouse_x, mouse_y, mouse_bstate);
    mvprintw(line++,0,          "debug %d,%d,%d,%d",debug_a,debug_b,debug_c,debug_d);

    bsu_simulation_write_count_age_location_velocity( bsu_start );

    iFrameCounter++;
    refresh();

    timespec end_time;
    clock_gettime(CLOCK_REALTIME, &end_time);

    frame_time     = timespec_sub( start_time, end_time ); 
    remaining_time = timespec_sub( frame_time, target_time );

    nanosleep( &remaining_time, NULL );

    timespec end_sleep_time;
    clock_gettime(CLOCK_REALTIME, &end_sleep_time);
    sleep_time = timespec_sub( end_time, end_sleep_time ); 

    timespec delta_time = timespec_add( frame_time, sleep_time );
    iTimeDelta = timespec_f( delta_time );
    iTime     += iTimeDelta;
  }

quit:
  printf("\033[?1003l\n"); // Disable mouse movement events, as l = low
  endwin();
  exit(0);
}

int mono_fragment( int u, int v )
{
  int result = 0;

  {
    int cx = offset_x;
    int cy = offset_y;
    int x  = u-cx;
    int y  = v-cy;
    int r  = 8; 
    if (( abs(x) <= (2*r) ) && ( abs(y) <= (2*r) ))
    {
      int d  = (int)sdPentagon( (float)x, (float)y, r );
      result |= d <= 0;
    }
  }

  {
    float t  = iTime * 5.0f;
    float vx = cosf(t)+sinf(t);
    float vy = -sinf(t)+cosf(t);
    float dx = vx * 20.0f;
    float dy = vy * 20.0f;
    int   cx   = (int)dx + offset_x;
    int   cy   = (int)dy + offset_y;
    int   x    = u - cx;
    int   y    = v - cy;
    int   r    = 5;
    if ( (abs(x) <= r) && (abs(y) <= r) )
    {
      result |= sdCircle( x, y, r ) <= 0;
    }
  }

  {
    float t  = (iTime * 5.0f)-0.5f;
    float vx = cosf(t)+sinf(t);
    float vy = -sinf(t)+cosf(t);
    float dx = vx * 20.0f;
    float dy = vy * 20.0f;
    int   cx   = (int)dx + offset_x;
    int   cy   = (int)dy + offset_y;
    int   x    = u - cx;
    int   y    = v - cy;
    int   r    = 4;
    if ( (abs(x) <= r) && (abs(y) <= r) )
    {
      result |= sdCircle( x, y, r ) <= 0;
    }
  }

  {
    int       cx        = iHalfWidth;
    int       cy        = iHalfHeight;
    int       x         = u-cx;
    int       y         = -(v-cy);
    uintptr_t bsu_start = (uintptr_t)_binary_simulation_bsu_start;

    result |= bsu_draw( bsu_start, x, y );
  }

  result |= ((u%64)==0);
  result |= ((v%64)==0);

  return result;
}

int
bsu_draw( uintptr_t bsu_start, int u, int v )
{
  static_array*  asset_spawn_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetSpawnOffset));
  struct_spawn*  asset_spawn_data          = (struct_spawn*)(bsu_start + asset_spawn_array->offset);
  static_array*  instance_location_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*  instance_location_data    = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*  instance_velocity_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*  instance_velocity_data    = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*  instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*  instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*  instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*      instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*  max_instance_count_array  = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*      max_instance_count_data   = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  int            asset_count               = asset_spawn_array->count;
  int            result                    = 0;
  int  line = 8;
 
  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    static_array*    location_array      = instance_location_data + asset_index;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;

    // mvprintw(line++,0,"asset:%d count:%d",asset_index,live_instance_count);
    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*    live_instance_age = age_data + live_instance_index;
      if ( *live_instance_age > 0 ) 
      {
        struct_vec2* live_location     = location_data + live_instance_index;
  
        float lx = live_location->x / 15.0f;
        float ly = live_location->y / 15.0f;
        int   cx = (int)(lx * iHalfWidth);
        int   cy = (int)(ly * iHalfHeight);
        int   x = u-cx;
        int   y = v-cy;

        int   r    = 5;
        if ( (abs(x) <= r) && (abs(y) <= r) )
        {
          result |= sdCircle( x, y, r ) <= 0;
        }
      }
    }
  }
  return result;
}

// ---------------------------------------------------------------------------------------


void
bsu_simulation_write_count_age_location_velocity( uintptr_t bsu_start )
{
  static_array*  asset_spawn_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetSpawnOffset));
  struct_spawn*  asset_spawn_data          = (struct_spawn*)(bsu_start + asset_spawn_array->offset);
  static_array*  asset_base_speed_array    = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSpeedOffset));
  float*         asset_base_speed_data     = (float*)(bsu_start + asset_base_speed_array->offset);
  static_array*  instance_location_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*  instance_location_data    = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*  instance_velocity_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*  instance_velocity_data    = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*  instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*  instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*  instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*      instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*  max_instance_count_array  = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*      max_instance_count_data   = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  static_array*  pattern_u8_array          = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kPatternU8Offset));
  uint8_t*       pattern_u8_data           = (uint8_t*)(bsu_start + pattern_u8_array->offset);
  static_array*  pattern_u32_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kPatternU32Offset));
  uint32_t*      pattern_u32_data          = (uint32_t*)(bsu_start + pattern_u32_array->offset);
  int            asset_count               = asset_spawn_array->count;
  float          t                         = iTime;
 
  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    struct_spawn*    spawn               = asset_spawn_data + asset_index;
    static_array*    at_each_array       = &spawn->at_each;
    uint32_t         at_each_count       = at_each_array->count;
    struct_at_each*  at_each_data        = (struct_at_each*)(bsu_start + at_each_array->offset);
    static_array*    at_group_array      = &spawn->at_group;
    uint32_t         at_group_count      = at_group_array->count;
    struct_at_group* at_group_data       = (struct_at_group*)(bsu_start + at_group_array->offset);

    float            base_speed          = asset_base_speed_data[ asset_index ];
    static_array*    location_array      = instance_location_data + asset_index;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*    velocity_array      = instance_velocity_data + asset_index;
    struct_vec2*     velocity_data       = (struct_vec2*)(bsu_start + velocity_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;

    // ---------------------------------------
    // Update instance locations
    // ---------------------------------------

    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*  live_instance_age = age_data + live_instance_index;
      if ( *live_instance_age > 0 ) 
      {
        struct_vec2* live_location   = location_data + live_instance_index;
        struct_vec2* live_velocity   = velocity_data + live_instance_index;

        live_location->x += live_velocity->x * iTimeDelta;
        live_location->y += live_velocity->y * iTimeDelta;

        *live_instance_age += iTimeDelta;
      }
    }

    // ---------------------------------------------------------------------------------
    // at_each: Spawn an instance for each live instance of a target asset type
    // ---------------------------------------------------------------------------------

    for (int at_each_index=0;at_each_index<at_each_count;at_each_index++)
    {
      struct_at_each*  at_each                      = at_each_data + at_each_index;
      float            time_next                    = at_each->time_next;
      float            time_step                    = at_each->time_step;
      struct_vec2*     location_offset              = &at_each->location_offset;
      uint32_t         target_asset_index_direction = at_each->target_index;
      uint32_t         target_asset_index           = target_asset_index_direction >> 2;
      uint32_t         initial_direction            = target_asset_index_direction & 0x03;
      uint32_t         target_instance_count        = instance_count_data[target_asset_index];
      uint32_t         target_max_instance_count    = max_instance_count_data[target_asset_index];
      uint32_t         target_live_instance_count   = ( target_instance_count > target_max_instance_count ) ? target_max_instance_count : target_instance_count;
      static_array*    target_age_array             = instance_age_data + target_asset_index;
      float*           target_age_data              = (float*)(bsu_start + target_age_array->offset);

      if (t >= time_next)
      {
        int            time_index              = (int)(t / time_step);
        uint32_t       target_instance_count   = instance_count_data[target_asset_index];
        static_array*  target_location_array   = instance_location_data + target_asset_index;
        struct_vec2*   target_location_data    = (struct_vec2*)(bsu_start + target_location_array->offset);

        uint32_t       pattern_width              = at_each->pattern_width;
        uint32_t       pattern_index              = time_index % pattern_width;
        uint32_t       pattern_index_word_index   = pattern_index >> 5;
        uint32_t       pattern_index_bit_index    = pattern_index & 0x1f;
        uint32_t       pattern_word_start_index   = at_each->pattern_u32_index;
        uint32_t       pattern_word               = pattern_u32_data[ pattern_word_start_index + pattern_index_word_index ];
        uint32_t       pattern_bit                = pattern_word & ( 1 << pattern_index_bit_index );

        if ( pattern_bit )
        {
          for (int target_instance_index=0;target_instance_index<target_live_instance_count;target_instance_index++)
          {
            float*         target_age = target_age_data + target_instance_index;
            if ( *target_age > 0.0f )
            {
              struct_vec2*   target_instance_location  = target_location_data + target_instance_index;
              uint32_t       next_instance_index       = instance_count % max_instance_count;
              struct_vec2*   next_instance_location    = location_data + next_instance_index;
              struct_vec2*   next_instance_velocity    = velocity_data + next_instance_index;
              float*         next_age                  = age_data + next_instance_index;
    
              next_instance_location->x = target_instance_location->x + location_offset->x;
              next_instance_location->y = target_instance_location->y + location_offset->y;
    
              float dir_x = 0.0f;
              float dir_y = 0.0f;
              switch (initial_direction)
              {
                case kBsuInitialDirectionDown:
                  dir_y = -1.0f;
                break;
    
                case kBsuInitialDirectionHero:
                case kBsuInitialDirectionAlongOffset:
                case kBsuInitialDirectionNone:
                default:
                break;
              }
      
              next_instance_velocity->x = dir_x * base_speed;
              next_instance_velocity->y = dir_y * base_speed;
    
              *next_age = FLT_MIN;
              instance_count++;
            }
          }
        }

        at_each->time_next = t + time_step;
      }

    }


    // ---------------------------------------------------------------------------------
    // at_group: Spawn an instance given an instance index of a target asset type
    // ---------------------------------------------------------------------------------

    for (int at_group_index=0;at_group_index<at_group_count;at_group_index++)
    {
      struct_at_group* at_group               = at_group_data + at_group_index;
      float            time_next              = at_group->time_next;
      float            time_start             = at_group->time_start;
      float            time_stop              = at_group->time_stop;
      float            time_step              = at_group->time_step;
      uint32_t         target_asset_index_direction = at_group->target_index;
      uint32_t         target_asset_index           = target_asset_index_direction >> 2;
      uint32_t         initial_direction      = target_asset_index_direction & 0x03;

      struct_vec2*     location_offset = &at_group->location_offset;

      if ((t >= time_next) && (t >= time_start) && (t <= time_stop))
      {
        int            time_index              = (int)((t-time_start) / time_step);
        uint32_t       target_instance_count   = instance_count_data[target_asset_index];
        static_array*  target_location_array   = instance_location_data + target_asset_index;
        struct_vec2*   target_location_data    = (struct_vec2*)(bsu_start + target_location_array->offset);
        uint32_t       pattern_count           = at_group->pattern_count;
        uint32_t       pattern_index           = at_group->pattern_u8_index + (time_index % pattern_count);
        uint32_t       target_instance_index   = pattern_u8_data[ pattern_index ];

        if ( target_instance_index < target_instance_count ) 
        {
          struct_vec2*   target_instance_location  = target_location_data + target_instance_index;
          uint32_t       next_instance_index       = instance_count % max_instance_count;
          struct_vec2*   next_instance_location    = location_data + next_instance_index;
          struct_vec2*   next_instance_velocity    = velocity_data + next_instance_index;
          float*         next_age                  = age_data + next_instance_index;

          next_instance_location->x = target_instance_location->x + location_offset->x;
          next_instance_location->y = target_instance_location->y + location_offset->y;

          float dir_x = 0.0f;
          float dir_y = 0.0f;
          switch (initial_direction)
          {
            case kBsuInitialDirectionDown:
              dir_y = -1.0f;
            break;

            case kBsuInitialDirectionHero:
            case kBsuInitialDirectionAlongOffset:
            case kBsuInitialDirectionNone:
            default:
            break;
          }
  
          next_instance_velocity->x = dir_x * base_speed;
          next_instance_velocity->y = dir_y * base_speed;

          *next_age = FLT_MIN;
          instance_count++;
        }

        at_group->time_next = t + time_step;
      }

    }

  
    // ---------------------------------------------------------------------------------
    // write instance count
    // ---------------------------------------------------------------------------------

    instance_count_data[asset_index] = instance_count;
  }
}
