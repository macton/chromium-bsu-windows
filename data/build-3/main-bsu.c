#include <locale.h>
#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>
#include "bsu.h"

#include <unistd.h> // usleep

#define imin(x,y) ((x)<(y))?(x):(y)
#define imax(x,y) ((x)>(y))?(x):(y)

// ---------------------------------------------------------------------------------------

extern char _binary_simulation_bsu_start[];
extern char _binary_simulation_bsu_size[];

void bsu_simulation_enable_hero( uintptr_t bsu_start );
void bsu_simulation_write_count_age_location_velocity( uintptr_t bsu_start );
void bsu_simulation_write_health( uintptr_t bsu_start );
int  bsu_draw( uintptr_t bsu_start );
void bsu_draw_write_event_time( uintptr_t bsu_start );

#define kEventDestroyTime 0.3f
#define kEventDestroyMaxCount 32

int         draw_event_destroy_count = 0;
float       draw_event_time_remaining[kEventDestroyMaxCount] = { 0.0f };

int debug_asset_index = 0;
int line = 0;

// ---------------------------------------------------------------------------------------

typedef struct timespec timespec;

int hero_location_x = 0;
int hero_location_y = 0;
float hero_x = 0;
float hero_y = 0;
uint32_t hero_flags = 0;

#define kHeroFlagTrigger0 1

float debug_a = 0.0f;
float debug_b = 0.0f;
float debug_c = 0.0f;
float debug_d = 0.0f;

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
  int64_t usec = (time.tv_nsec - (msec*1000000)) / 1000;
  int64_t nsec = time.tv_nsec - (msec*1000000) - (usec*1000);

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

int fragment_main( int x, int y );

#define FRAMEBUFFER_WIDTH  512
#define FRAMEBUFFER_HEIGHT 512
#define FRAMEBUFFER_SIZE   (FRAMEBUFFER_WIDTH*FRAMEBUFFER_HEIGHT)
uint8_t framebuffer[ FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT ];

void
framebuffer_clear(void)
{
  memset(framebuffer,0,FRAMEBUFFER_SIZE);
}

int framebuffer_bitmask[8] = { 1, 8, 2, 16, 4, 32, 64, 128 };

uint8_t
framebuffer_byte_read(int x, int y)
{
  if ( x < 0 ) return 0;
  if ( y < 0 ) return 0;

  int char_x             = x/2;
  int char_y             = y/4;
  int char_index         = (char_y * FRAMEBUFFER_WIDTH)+char_x;
  int char_value         = framebuffer[ char_index ];
  return char_value;
}

void
framebuffer_byte_write(int x, int y, uint8_t value)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;

  int char_x             = x/2;
  int char_y             = y/4;
  int char_index         = (char_y * FRAMEBUFFER_WIDTH)+char_x;

  framebuffer[ char_index ] = value;
}

uint8_t
framebuffer_bit_read(int x, int y)
{
  if ( x < 0 ) return 0;
  if ( y < 0 ) return 0;

  int char_x             = x/2;
  int char_y             = y/4;
  int char_index         = (char_y * FRAMEBUFFER_WIDTH)+char_x;
  int char_value         = framebuffer[ char_index ];
  int char_ox            = x - (char_x*2);
  int char_oy            = y - (char_y*4);
  int char_bitmask_index = (char_oy*2)+char_ox;
  int char_bitmask       = framebuffer_bitmask[ char_bitmask_index ];
  return char_value & char_bitmask;
}

void
framebuffer_bit_write(int x, int y)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;
  
  int char_x             = x/2;
  int char_y             = y/4;
  int char_index         = (char_y * FRAMEBUFFER_WIDTH)+char_x;
  int char_value         = framebuffer[ char_index ];
  int char_ox            = x - (char_x*2);
  int char_oy            = y - (char_y*4);
  int char_bitmask_index = (char_oy*2)+char_ox;
  int char_bitmask       = framebuffer_bitmask[ char_bitmask_index ];

  framebuffer[ char_index ] |= char_bitmask;
}

void
framebuffer_bit_clear(int x, int y)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;

  int char_x             = x/2;
  int char_y             = y/4;
  int char_index         = (char_y * FRAMEBUFFER_WIDTH)+char_x;
  int char_value         = framebuffer[ char_index ];
  int char_ox            = x - (char_x*2);
  int char_oy            = y - (char_y*4);
  int char_bitmask_index = (char_oy*2)+char_ox;
  int char_bitmask       = framebuffer_bitmask[ char_bitmask_index ];

  framebuffer[ char_index ] &= ~char_bitmask;
}

void
framebuffer_draw_circle(int cx, int cy, int r)
{
  int x0 = cx-r;
  int y0 = cy-r;
  int x1 = cx+r;
  int y1 = cy+r;
  int lcx = (x0+x1)/2;
  int lcy = (y0+y1)/2;

  for (int ly=y0;ly<=y1;ly++)
  {
    for (int lx=x0;lx<=x1;lx++)
    {
      int x = lx - lcx;
      int y = ly - lcy;
      int d = sdCircle( x, y, r);
      if ( d <= 0 )
      {
        framebuffer_bit_write( lx, ly );
      }
    }
  }
}


int main(void)
{
  // enable support UTF-8
  setlocale(LC_ALL, "");
  
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
  set_escdelay(0);

  mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
  mouseinterval(0);

  // xterm SET_ANY_EVENT_MOUSE
  // see: https://www.xfree86.org/current/ctlseqs.html#Mouse%20Tracking
  printf("\033[?1003h\n"); // Makes the terminal report mouse movement events
  fflush(stdout);

  // start_color();

  clear();

  iWidth      = COLS * 2;
  iHeight     = LINES * 4;
  iHalfWidth  = iWidth / 2;
  iHalfHeight = iHeight / 2;


#if 0
  int offset = 0;
  do
  {
    int color_id = 0;
    
    for (int y=0;y<16;y++) 
    {
      for (int x=0;x<16;x++) 
      {
        int r = 250+(x*750)/16;
        int b = 250+(y*750)/16;
        int g = imin(r,b);

if (color_id == 0)
{
  r = 0;
  g = 0;
  b = 0;
}


        init_color(color_id, r, g, b);
        init_pair(color_id, color_id, COLOR_BLACK);
        attron( COLOR_PAIR(color_id) );
        mvprintw(y,x,"*");
        attroff( COLOR_PAIR(color_id) );
       

        color_id++;
      }
    }

    refresh();
    offset = (offset+1)%1000;
    usleep(10000 );
  }
  while (getch() != ' ');
#endif

  timespec frame_time;
  timespec sleep_time;
  timespec remaining_time;
  timespec target_time = { 0, 16666666 }; 

  uintptr_t bsu_start = (uintptr_t)_binary_simulation_bsu_start;

  bsu_simulation_enable_hero( bsu_start );

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
          hero_location_x -= 10;
        break;
        case KEY_RIGHT:
          hero_location_x += 10;
        break;
        case KEY_UP:
          hero_location_y -= 10;
        break;
        case KEY_DOWN:
          hero_location_y += 10;
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
    
            hero_location_x = mouse_x * 2;
            hero_location_y = mouse_y * 4;
            hero_x = 2.0f * 15.0f * (((float)hero_location_x / (float)iWidth) - 0.5f);
            hero_y = -2.0f * 15.0f * (((float)hero_location_y / (float)iHeight) - 0.5f);

            if ( event.bstate & BUTTON1_RELEASED )
            {
              hero_flags &= ~(1 << kHeroFlagTrigger0);
            }
            if ( event.bstate & BUTTON1_PRESSED )
            {
              hero_flags |= 1 << kHeroFlagTrigger0;
            }

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

    line = 0;

    timespec draw_time;
    timespec present_time;
    timespec sim_time;

    {
      timespec start_time;
      timespec end_time;

      clock_gettime(CLOCK_REALTIME, &start_time);
      bsu_simulation_write_count_age_location_velocity( bsu_start );
      bsu_simulation_write_health( bsu_start );
      clock_gettime(CLOCK_REALTIME, &end_time);
      sim_time = timespec_sub( start_time, end_time );
    }


    {
      timespec start_time;
      timespec end_time;

      clock_gettime(CLOCK_REALTIME, &start_time);

      framebuffer_clear();
      bsu_draw_write_event_time(bsu_start);
      bsu_draw( bsu_start );

      clock_gettime(CLOCK_REALTIME, &end_time);
      draw_time = timespec_sub( start_time, end_time );
    }

    {
      timespec start_time;
      timespec end_time;

      clock_gettime(CLOCK_REALTIME, &start_time);

      for (int y=0;y<iHeight;y+=4)
      {
        for (int x=0;x<iWidth;x+=2)
        {
          int ch = framebuffer_byte_read(x,y);
          mvprintw(y/4,x/2,"%lc",L'\u2800'+ch);
        }
      }

      clock_gettime(CLOCK_REALTIME, &end_time);
      present_time = timespec_sub( start_time, end_time );
    }


    mvprint_timespec( line++,0, "frame_time:      ", frame_time);
    mvprint_timespec( line++,0, "sleep_time:      ", sleep_time);
    mvprint_timespec( line++,0, "sim_time:      ", sim_time);
    mvprint_timespec( line++,0, "draw_time:     ", draw_time);
    mvprint_timespec( line++,0, "present_time:  ", present_time);
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
    mvprintw(line++,0,          "debug %f,%f,%f,%f",debug_a,debug_b,debug_c,debug_d);


    mvprintw(line++,0,          "hero_location  %d,%d",hero_location_x,hero_location_y);
    mvprintw(line++,0,          "hero  %f,%f",hero_x,hero_y);

    {
      static_array*  instance_health_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceHealthOffset));
      static_array*  instance_health_data         = (static_array*)(bsu_start + instance_health_array->offset);
      static_array*  health_array                 = instance_health_data + 0;
      float*         health_data                  = (float*)(bsu_start + health_array->offset );
      float          hero_health                  = health_data[0];

      mvprintw(line++,0,          "hero_health:  %f",hero_health);
    }

    {
      struct_event_destroyed_at*    event_destroyed_at_data           = (struct_event_destroyed_at*)(bsu_start + *(uint32_t*)(bsu_start + kEventDestroyedAtOffset));
      uint32_t                      event_destroyed_at_count          = event_destroyed_at_data->count;

      mvprintw(line++,0,          "event_destroy_count:  %d",event_destroyed_at_count);
    }

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

int
bsu_draw( uintptr_t bsu_start )
{
  static_array*  asset_spawn_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetSpawnOffset));
  struct_spawn*  asset_spawn_data          = (struct_spawn*)(bsu_start + asset_spawn_array->offset);
  static_array*  asset_base_size_array    = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSizeOffset));
  struct_vec2*   asset_base_size_data     = (struct_vec2*)(bsu_start + asset_base_size_array->offset);
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
 
  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    struct_vec2*     base_size           = asset_base_size_data + asset_index;
    static_array*    location_array      = instance_location_data + asset_index;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;

    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*    live_instance_age = age_data + live_instance_index;
      if ( *live_instance_age > 0 ) 
      {
        struct_vec2* live_location     = location_data + live_instance_index;
  
        float lx = (live_location->x) / 15.0f;
        float ly = (live_location->y) / 15.0f;
        int   cx = iHalfWidth  + (int)(lx * iHalfWidth);
        int   cy = iHeight-(iHalfHeight + (int)(ly * iHalfHeight));
        int   r    = ((base_size->x / 15.0) * iHalfWidth);

        debug_asset_index = asset_index;
        framebuffer_draw_circle(cx, cy, r);
      }
    }
  }

  // ---------------------------------------------------------------------------------
  // draw destroy effects
  // ---------------------------------------------------------------------------------
  struct_event_destroyed_at*    event_destroyed_at_data           = (struct_event_destroyed_at*)(bsu_start + *(uint32_t*)(bsu_start + kEventDestroyedAtOffset));
  uint32_t                      event_destroyed_at_count          = event_destroyed_at_data->count;
  static_array*                 event_destroyed_at_location_array = &event_destroyed_at_data->at;
  struct_vec2*                  event_destroyed_at_location_data  = (struct_vec2*)(bsu_start + event_destroyed_at_location_array->offset);

  for (int draw_event_destroy_index=0;draw_event_destroy_index<kEventDestroyMaxCount;draw_event_destroy_index++)
  {
    float time_remaining = draw_event_time_remaining[draw_event_destroy_index];
    if (time_remaining > 0.0f)
    {
      struct_vec2 location   = event_destroyed_at_location_data[draw_event_destroy_index];
      float t                = (kEventDestroyTime-time_remaining)/kEventDestroyTime;
  
      float lx = location.x / 15.0f;
      float ly = location.y / 15.0f;
      int   cx = iHalfWidth  + (int)(lx * iHalfWidth);
      int   cy = iHeight-(iHalfHeight + (int)(ly * iHalfHeight));
      float start_size = 2.0f;
      float stop_size  = 3.0f;
      float size = start_size + (t*(stop_size-start_size));
      int   r = ((size / 15.0) * iHalfWidth);

      framebuffer_draw_circle(cx, cy, r);
    }
  }

  return result;
}

void
bsu_draw_write_event_time( uintptr_t bsu_start )
{
  struct_event_destroyed_at*    event_destroyed_at_data           = (struct_event_destroyed_at*)(bsu_start + *(uint32_t*)(bsu_start + kEventDestroyedAtOffset));
  uint32_t                      event_destroyed_at_count          = event_destroyed_at_data->count;

  // ---------------------------------------------------------------------------------
  // Update timer for destroy effects
  // ---------------------------------------------------------------------------------
  for (int draw_event_destroy_index=0;draw_event_destroy_index<kEventDestroyMaxCount;draw_event_destroy_index++)
  {
    float time_remaining = draw_event_time_remaining[draw_event_destroy_index];
    if (time_remaining > 0.0f)
    {
      draw_event_time_remaining[draw_event_destroy_index] -= iTimeDelta;
    }
  }

  // ---------------------------------------------------------------------------------
  // add new destroy effects
  // ---------------------------------------------------------------------------------
  while ( draw_event_destroy_count < event_destroyed_at_count)
  {
    draw_event_time_remaining[draw_event_destroy_count%kEventDestroyMaxCount] = kEventDestroyTime;
    draw_event_destroy_count++;
  }

}

// ---------------------------------------------------------------------------------------

void
bsu_simulation_enable_hero( uintptr_t bsu_start )
{
  static_array*  instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*      instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*  instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*  instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*  age_array                 = instance_age_data + 0;
  float*         age_data                  = (float*)(bsu_start + age_array->offset );

  instance_count_data[0] = 1;
  age_data[0] = FLT_MIN;
}

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
  static_array*  instance_health_array     = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceHealthOffset));
  static_array*  instance_health_data      = (static_array*)(bsu_start + instance_health_array->offset);
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

  // Hero Asset (0)
  {
    static_array*    location_array      = instance_location_data + 0;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);

    location_data->x = hero_x;
    location_data->y = hero_y;
  }

  // ---------------------------------------------------------------------------------------
  // Update instance age
  // ---------------------------------------------------------------------------------------
  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;

    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*  live_instance_age = age_data + live_instance_index;
      if ( *live_instance_age > 0 ) 
      {
        *live_instance_age += iTimeDelta;
      }
    }
  }
 
  for (int asset_index=1;asset_index<asset_count;asset_index++)
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

    static_array*    health_array        = instance_health_data + asset_index;
    float*           health_data         = (float*)(bsu_start + health_array->offset);

    static_array*    velocity_array      = instance_velocity_data + asset_index;
    struct_vec2*     velocity_data       = (struct_vec2*)(bsu_start + velocity_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;

    // ---------------------------------------------------------------------------------------
    // Update instance location and age
    // ---------------------------------------------------------------------------------------

    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*  live_age    = age_data + live_instance_index;
      float*  live_health = health_data + live_instance_index;

      if ( *live_age > 0 ) 
      {
        struct_vec2* live_location   = location_data + live_instance_index;
        struct_vec2* live_velocity   = velocity_data + live_instance_index;

        live_location->x += live_velocity->x * iTimeDelta;
        live_location->y += live_velocity->y * iTimeDelta;

        int out_band    = ((live_location->x < -17.0f) || (live_location->x > 17.0f) || (live_location->y < -17.0f) || (live_location->y > 17.0f));
        int out_health  = *live_health <= 0.0f;
        int end_of_life = out_band || out_health;
        *live_age = (end_of_life)?0.0f:(*live_age + iTimeDelta);
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
      uint32_t         target_asset_index           = target_asset_index_direction >> 4;
      uint32_t         initial_direction            = target_asset_index_direction & 0x03;
      uint32_t         on_flag                      = (target_asset_index_direction >> 2) & 0x03;
      uint32_t         target_instance_count        = instance_count_data[target_asset_index];
      uint32_t         target_max_instance_count    = max_instance_count_data[target_asset_index];
      uint32_t         target_live_instance_count   = ( target_instance_count > target_max_instance_count ) ? target_max_instance_count : target_instance_count;
      static_array*    target_age_array             = instance_age_data + target_asset_index;
      float*           target_age_data              = (float*)(bsu_start + target_age_array->offset);

      if ( on_flag == kBsuOnFlagHeroTrigger0 )
      {
        if ( (hero_flags & (1 << kHeroFlagTrigger0)) == 0) 
        {
          continue;
        }
      }

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
              struct_vec2*   target_location  = target_location_data + target_instance_index; 
              uint32_t       next_instance_index       = instance_count % max_instance_count; 
              struct_vec2*   next_instance_location    = location_data + next_instance_index;
              struct_vec2*   next_instance_velocity    = velocity_data + next_instance_index;
              float*         next_age                  = age_data + next_instance_index;
    
              next_instance_location->x = target_location->x + location_offset->x;
              next_instance_location->y = target_location->y + location_offset->y;
    
              float dir_x = 0.0f;
              float dir_y = 0.0f;
              switch (initial_direction)
              {
                case kBsuInitialDirectionDown:
                  dir_y = -1.0f;
                break;
                case kBsuInitialDirectionUp:
                  dir_y = 1.0f;
                break;
                case kBsuInitialDirectionHero:
                {
                  float dx = hero_x - next_instance_location->x;
                  float dy = hero_y - next_instance_location->y;
                  float len = sqrtf( (dx*dx)+(dy*dy) );
                  dir_x = dx/len;
                  dir_y = dy/len;
                }
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
      struct_at_group* at_group                     = at_group_data + at_group_index;
      float            time_next                    = at_group->time_next;
      float            time_start                   = at_group->time_start;
      float            time_stop                    = at_group->time_stop;
      float            time_step                    = at_group->time_step;
      struct_vec2*     location_offset              = &at_group->location_offset;
      uint32_t         target_asset_index_direction = at_group->target_index;
      uint32_t         target_asset_index           = target_asset_index_direction >> 4;
      uint32_t         initial_direction            = target_asset_index_direction & 0x03;
      uint32_t         on_flag                      = (target_asset_index_direction >> 2) & 0x03;

      if ( on_flag == kBsuOnFlagHeroTrigger0 )
      {
        if ( (hero_flags & (1 << kHeroFlagTrigger0)) == 0) 
        {
          continue;
        }
      }

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
          struct_vec2*   target_location  = target_location_data + target_instance_index;
          uint32_t       next_instance_index       = instance_count % max_instance_count;
          struct_vec2*   next_instance_location    = location_data + next_instance_index;
          struct_vec2*   next_instance_velocity    = velocity_data + next_instance_index;
          float*         next_age                  = age_data + next_instance_index;

          next_instance_location->x = target_location->x + location_offset->x;
          next_instance_location->y = target_location->y + location_offset->y;

          float dir_x = 0.0f;
          float dir_y = 0.0f;
          switch (initial_direction)
          {
            case kBsuInitialDirectionDown:
              dir_y = -1.0f;
            break;
            case kBsuInitialDirectionUp:
              dir_y = 1.0f;
            break;
            case kBsuInitialDirectionHero:
            {
              float dx = hero_x - next_instance_location->x;
              float dy = hero_y - next_instance_location->y;
              float len = sqrtf( (dx*dx)+(dy*dy) );

              dir_x = dx/len;
              dir_y = dy/len;
            }
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

void
bsu_simulation_write_health( uintptr_t bsu_start )
{
  static_array*  asset_base_health_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseHealthOffset));
  float*         asset_base_health_data    = (float*)(bsu_start + asset_base_health_array->offset);
  static_array*  asset_base_size_array    = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSizeOffset));
  struct_vec2*   asset_base_size_data     = (struct_vec2*)(bsu_start + asset_base_size_array->offset);
  int            asset_count               = asset_base_health_array->count;
  static_array*  instance_location_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*  instance_location_data    = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*  instance_velocity_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*  instance_velocity_data    = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*  instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*  instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*  instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*      instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);

  static_array*  instance_health_array     = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceHealthOffset));
  static_array*  instance_health_data      = (static_array*)(bsu_start + instance_health_array->offset);
  static_array*  max_instance_count_array  = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*      max_instance_count_data   = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  float          t                         = iTime;

  static_array*                 collision_mod_health_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kCollisionModHealthOffset));
  struct_collision_mod_health*  collision_mod_health_data         = (struct_collision_mod_health*)(bsu_start + collision_mod_health_array->offset);
  int                           collision_mod_health_source_count = collision_mod_health_array->count;

  struct_event_destroyed_at*    event_destroyed_at_data           = (struct_event_destroyed_at*)(bsu_start + *(uint32_t*)(bsu_start + kEventDestroyedAtOffset));
  uint32_t                      event_destroyed_at_count          = event_destroyed_at_data->count;
  static_array*                 event_destroyed_at_location_array = &event_destroyed_at_data->at;
  struct_vec2*                  event_destroyed_at_location_data  = (struct_vec2*)(bsu_start + event_destroyed_at_location_array->offset);

  // ---------------------------------------------------------------------------------
  // Write to health on collision (all instances of source vs all instances of target)
  // Write to event_destroyed_at
  // ---------------------------------------------------------------------------------

  for (int source_index=0;source_index<collision_mod_health_source_count;source_index++)
  {
    struct_collision_mod_health*         collision_mod_health = collision_mod_health_data + source_index;
    uint32_t                             source_asset_index   = collision_mod_health->source_asset_index;
    uint32_t                             targets_offset       = collision_mod_health->targets.offset;
    uint32_t                             targets_count        = collision_mod_health->targets.count;
    struct_collision_mod_health_target*  targets_data         = (struct_collision_mod_health_target*)(bsu_start + targets_offset);

    static_array*    source_age_array           = instance_age_data + source_asset_index;
    float*           source_age_data            = (float*)(bsu_start + source_age_array->offset );
    struct_vec2      source_base_size           = asset_base_size_data[ source_asset_index ];

    static_array*    source_location_array      = instance_location_data + source_asset_index;
    struct_vec2*     source_location_data       = (struct_vec2*)(bsu_start + source_location_array->offset);

    static_array*    source_health_array        = instance_health_data + source_asset_index;
    float*           source_health_data         = (float*)(bsu_start + source_health_array->offset);

    uint32_t         source_instance_count      = instance_count_data[source_asset_index];
    uint32_t         source_max_instance_count  = max_instance_count_data[source_asset_index];

    uint32_t         source_live_instance_count = ( source_instance_count > source_max_instance_count ) ? source_max_instance_count : source_instance_count;

    for (int source_instance_index=0;source_instance_index<source_live_instance_count;source_instance_index++)
    {
      float*        source_age      = source_age_data + source_instance_index;
      struct_vec2*  source_location = source_location_data + source_instance_index;
      float*        source_health   = source_health_data + source_instance_index;

      if ( *source_age > 0)
      {
        for (int target_index=0;target_index<targets_count;target_index++)
        {
          struct_collision_mod_health_target*  target                   = targets_data + target_index;
          uint32_t                             target_asset_index       = target->target_asset_index;
          float                                target_mod_health_amount = target->amount;
    
          static_array*                        target_age_array           = instance_age_data + target_asset_index;
          float*                               target_age_data            = (float*)(bsu_start + target_age_array->offset );
          struct_vec2                          target_base_size           = asset_base_size_data[ target_asset_index ];
          static_array*                        target_location_array      = instance_location_data + target_asset_index;
          struct_vec2*                         target_location_data       = (struct_vec2*)(bsu_start + target_location_array->offset);
          static_array*                        target_health_array        = instance_health_data + target_asset_index;
          float*           target_health_data         = (float*)(bsu_start + target_health_array->offset);

          uint32_t         target_instance_count      = instance_count_data[target_asset_index];
          uint32_t         target_max_instance_count  = max_instance_count_data[target_asset_index];
          uint32_t         target_live_instance_count = ( target_instance_count > target_max_instance_count ) ? target_max_instance_count : target_instance_count;
      
          for (int target_instance_index=0;target_instance_index<target_live_instance_count;target_instance_index++)
          {
            float*        target_age      = target_age_data + target_instance_index;
            struct_vec2*  target_location = target_location_data + target_instance_index;
            float*        target_health   = target_health_data + target_instance_index;
      
            if ( *target_age > 0)
            {
              float  r        = source_base_size.x + target_base_size.x;
              float  dx       = source_location->x - target_location->x;
              float  dy       = source_location->y - target_location->y;
              float  center_d = sqrtf( (dx*dx)+(dy*dy) );
              float  d        = center_d - r;
              if ( d <= 0.0f) 
              {
                *target_health += target_mod_health_amount;
                *source_health  = 0.0f; // self-destruct
                if ( *target_health <= 0.0f ) {
                  event_destroyed_at_location_data[event_destroyed_at_count % kEventDestroyedAtMaxCount] = *target_location;
                  event_destroyed_at_count++;
                }
              }
            }
          }
        }
      }
    }
  }

  event_destroyed_at_data->count = event_destroyed_at_count;

  // ---------------------------------------------------------------------------------
  // initialize instance health
  // ---------------------------------------------------------------------------------

  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    float            base_health         = asset_base_health_data[ asset_index ];
    static_array*    health_array        = instance_health_data + asset_index;
    float*           health_data         = (float*)(bsu_start + health_array->offset);
    static_array*    location_array      = instance_location_data + asset_index;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*    velocity_array      = instance_velocity_data + asset_index;
    struct_vec2*     velocity_data       = (struct_vec2*)(bsu_start + velocity_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;
   
    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*  live_instance_age = age_data + live_instance_index;
      if ( *live_instance_age == FLT_MIN )
      {
        float* live_health = health_data + live_instance_index;
        *live_health = base_health;
      }
    }
  }
}
