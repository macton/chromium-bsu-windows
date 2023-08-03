#include <locale.h>
#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <string.h>
#include "bsu.h"
#include "bitrect.h"
#include "timespec_util.h"
#include "f32_math.h"
#include "s32_math.h"
#include "bsu_simulation_runtime.h"

// ---------------------------------------------------------------------------------------

extern char _binary_simulation_bsu_start[];
extern char _binary_simulation_bsu_size[];

int  bsu_draw( uintptr_t bsu_start );
void bsu_draw_write_event_time( uintptr_t bsu_start );

#define kEventDestroyTime 0.2f
#define kEventDestroyMaxCount 32

int         draw_event_destroy_count = 0;
float       draw_event_time_remaining[kEventDestroyMaxCount] = { 0.0f };

int debug_asset_index = 0;
int line = 0;

// ---------------------------------------------------------------------------------------


int      hero_location_x = 0;
int      hero_location_y = 0;
float    hero_x          = 0.0f;
float    hero_y          = 0.0f;
uint32_t hero_flags      = 0;


float debug_a = 0.0f;
float debug_b = 0.0f;
float debug_c = 0.0f;
float debug_d = 0.0f;


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

typedef struct bitrect_framebuffer bitrect_framebuffer;
struct bitrect_framebuffer
{
  int32_t width;
  int32_t height;
  uint8_t buffer[ bitrect_calcsize( FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT ) ];
};

bitrect_framebuffer framebuffer = { FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT, 0 };

void quit(void)
{
  printf("\033[?1003l\n"); // Disable mouse movement events, as l = low
  endwin();
  exit(0);
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
  printf("\033[?1003h\n"); 
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
          quit();
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
              hero_flags &= ~kHeroTrigger0;
            }
            if ( event.bstate & BUTTON1_PRESSED )
            {
              hero_flags |= kHeroTrigger0;
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

      bsu_simulation_write_play_time( bsu_start, iTime, iTimeDelta, iFrameCounter );
      bsu_simulation_write_hero_flags( bsu_start, hero_flags );
      bsu_simulation_write_hero_location( bsu_start, hero_x, hero_y );
      bsu_simulation_write_count_age_location_velocity( bsu_start );
      bsu_simulation_write_health( bsu_start );
      bsu_simulation_write_mod_flags( bsu_start );

      clock_gettime(CLOCK_REALTIME, &end_time);
      sim_time = timespec_sub( start_time, end_time );
    }


    {
      timespec start_time;
      timespec end_time;
      clock_gettime(CLOCK_REALTIME, &start_time);

      bitrect_clear((bitrect_buffer*)&framebuffer);
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
        move(y/4,0);
        for (int x=0;x<iWidth;x+=2)
        {
          int ch = bitrect_byte_read((bitrect_buffer*)&framebuffer,x,y);
          wchar_t str[2];
          str[0] = L'\u2800'+ch;
          str[1] = 0;
          // mvprintw(y/4,x/2,"%lc",L'\u2800'+ch);
          addwstr(str); 
        }
      }

      clock_gettime(CLOCK_REALTIME, &end_time);
      present_time = timespec_sub( start_time, end_time );
    }


    mvprint_timespec( line++,0, "frame_time:      ", frame_time);
    mvprint_timespec( line++,0, "sleep_time:      ", sleep_time);
    mvprint_timespec( line++,0, "sim_time:        ", sim_time);
    mvprint_timespec( line++,0, "draw_time:       ", draw_time);
    mvprint_timespec( line++,0, "present_time:    ", present_time);
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

      static_array*  hero_health_array            = instance_health_data + kHeroIndex;
      float*         hero_health_data             = (float*)(bsu_start + hero_health_array->offset );
      float          hero_health                  = hero_health_data[0];

      static_array*  hero_shield_health_array     = instance_health_data + kHeroShieldIndex;
      float*         hero_shield_health_data      = (float*)(bsu_start + hero_shield_health_array->offset );
      float          hero_shield_health           = hero_shield_health_data[0];

      mvprintw(line++,0,          "hero_health:  %f",hero_health);
      mvprintw(line++,0,          "hero_shield_health:  %f",hero_shield_health);
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

    //timespec delta_time = timespec_add( frame_time, sleep_time );
    timespec delta_time = timespec_add( frame_time, sleep_time );
    iTimeDelta = timespec_f( delta_time );
    iTime     += iTimeDelta;
  }
}

int
bsu_draw( uintptr_t bsu_start )
{
  static_array*  asset_spawn_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetSpawnOffset));
  struct_spawn*  asset_spawn_data          = (struct_spawn*)(bsu_start + asset_spawn_array->offset);
  static_array*  asset_base_size_array     = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSizeOffset));
  struct_vec2*   asset_base_size_data      = (struct_vec2*)(bsu_start + asset_base_size_array->offset);
  static_array*  instance_location_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*  instance_location_data    = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*  instance_velocity_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*  instance_velocity_data    = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*  instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*  instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*  instance_health_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceHealthOffset));
  static_array*  instance_health_data         = (static_array*)(bsu_start + instance_health_array->offset);
  static_array*  instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*      instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*  max_instance_count_array  = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*      max_instance_count_data   = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  int            asset_count               = asset_spawn_array->count;
  int            result                    = 0;

  {
    float x = 7.0f;
    float y = 3.0f;
    float r0 = 1.0f;
    float lx = (x) / 15.0f;
    float ly = (y) / 15.0f;
    int   cx = iHalfWidth  + (int)(lx * iHalfWidth);
    int   cy = iHeight-(iHalfHeight + (int)(ly * iHalfHeight));
    int   r  = (int)((r0 / 15.0f) * (float)iHalfWidth);

    bitrect_draw_circle_2((bitrect_buffer*)&framebuffer, cx,cy,r);
  }

  {
    float x = 7.0f;
    float y = 6.5f;
    float r0 = 1.0f;
    float lx = (x) / 15.0f;
    float ly = (y) / 15.0f;
    int   cx = iHalfWidth  + (int)(lx * iHalfWidth);
    int   cy = iHeight-(iHalfHeight + (int)(ly * iHalfHeight));
    int   r  = (int)((r0 / 15.0f) * (float)iHalfWidth);

    bitrect_draw_circle_2((bitrect_buffer*)&framebuffer, cx,cy,r);
  }
 
  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    struct_vec2*     base_size           = asset_base_size_data + asset_index;
    static_array*    location_array      = instance_location_data + asset_index;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    static_array*    health_array        = instance_health_data + asset_index;
    float*           health_data         = (float*)(bsu_start + health_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;

    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*    live_instance_age    = age_data + live_instance_index;
      float*    live_instance_health = health_data + live_instance_index;

      if ((*live_instance_age > 0) && (*live_instance_health > 0))
      {
        struct_vec2* live_location     = location_data + live_instance_index;
  
        float lx = (live_location->x) / 15.0f;
        float ly = (live_location->y) / 15.0f;
        int   cx = iHalfWidth  + (int)(lx * iHalfWidth);
        int   cy = iHeight-(iHalfHeight + (int)(ly * iHalfHeight));
        int   r  = (int)((base_size->x / 15.0f) * (float)iHalfWidth);

        debug_asset_index = asset_index;
        if ( asset_index == kHeroShieldIndex ) 
        {
          bitrect_draw_circle_border((bitrect_buffer*)&framebuffer,cx, cy, r, 1);
          // bitrect_draw_circle_2((bitrect_buffer*)&framebuffer,cx, cy, r);
        }
        else
        {
          bitrect_draw_circle((bitrect_buffer*)&framebuffer,cx, cy, r);
        }
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
      float start_size = 0.5f;
      float stop_size  = 2.0f;
      float size = start_size + (t*(stop_size-start_size));
      int   r = ((size / 15.0) * iHalfWidth);

      bitrect_draw_circle_2((bitrect_buffer*)&framebuffer,cx, cy, r);
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


