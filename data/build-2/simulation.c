#include <locale.h>
#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <float.h>

// ---------------------------------------------------------------------------------------

extern char _binary_simulation_config_bsu_start[];
extern char _binary_simulation_config_bsu_size[];

typedef struct name_string name_string;
typedef struct toc_offsets toc_offsets;
typedef struct spawn_at_group spawn_at_group;
typedef struct spawn_at_each spawn_at_each;
typedef struct vec2 vec2;
typedef struct reference_spawn reference_spawn;
typedef struct bsu bsu;

struct name_string
{
  char utf8[64];
};

struct toc_offsets
{
  uint32_t reference_offset;
  uint32_t max_instance_count_offset;
  uint32_t instance_count_offset;
  uint32_t playarea_offset;
  uint32_t location_offset;
  uint32_t base_size_offset;
  uint32_t base_speed_offset;
  uint32_t spawn_offset;
  uint32_t age_offset;
  uint32_t data_u8_offset;
  uint32_t data_u32_offset;
  uint32_t data_at_each_offset;
  uint32_t data_at_group_offset;
  uint32_t data_vec2_offset;
  uint32_t data_f32_offset;
};

struct reference_spawn
{
  uint32_t at_each_offset;
  uint32_t at_each_count;
  uint32_t at_group_offset;
  uint32_t at_group_count;
};

struct vec2
{
  float x;
  float y;
};

struct spawn_at_each
{
  float    next_spawn_time;
  float    time_step;
  vec2     offset;
  uint32_t pattern_width;
  uint32_t pattern_data_u32_offset;
};

struct spawn_at_group
{
  float    next_spawn_time;
  float    time_step;
  float    time_start;
  float    time_stop;
  vec2     offset;
  uint32_t pattern_count;
  uint32_t pattern_data_u8_offset;
};

struct bsu
{
  name_string magic;
  toc_offsets toc_offsets;  
};

uint32_t bsu_reference_count(bsu* bsu_start) 
{
  uintptr_t start_addr      = (uintptr_t)bsu_start;
  uintptr_t reference_addr  = start_addr + bsu_start->toc_offsets.reference_offset;
  uint32_t  reference_count = *(uint32_t*)reference_addr;
  return reference_count;
}

uint32_t bsu_spawn_at_each_count(bsu* bsu_start, uint32_t reference_index)
{
  uintptr_t        start_addr  = (uintptr_t)bsu_start;
  uintptr_t        spawn_addr  = start_addr + bsu_start->toc_offsets.spawn_offset;
  reference_spawn* spawn       = (reference_spawn*)spawn_addr;
  return spawn[reference_index].at_each_count;
}

uint32_t bsu_spawn_at_group_count(bsu* bsu_start, uint32_t reference_index)
{
  uintptr_t        start_addr  = (uintptr_t)bsu_start;
  uintptr_t        spawn_addr  = start_addr + bsu_start->toc_offsets.spawn_offset;
  reference_spawn* spawn       = (reference_spawn*)spawn_addr;
  return spawn[reference_index].at_group_count;
}

uint32_t bsu_spawn_at_each_count_(bsu* bsu_start, uint32_t reference_index)



// ---------------------------------------------------------------------------------------

typedef struct timespec timespec;

int offset_x = 0;
int offset_y = 0;

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
  int64_t sec  = time.tv_sec;
  int64_t msec = time.tv_nsec / 1000000;
  int64_t usec = (time.tv_nsec - (msec*1000000)) / 10000;
  int64_t nsec = time.tv_nsec - (msec*1000000) - (usec*10000);
  float   secf = ((float)sec)
               + ((float)msec)/10000.0f
               + ((float)usec)/1000000.0f
               + ((float)nsec)/1000000000.0f;
  return secf;
}

// ---------------------------------------------------------------------------------------

int   iWidth        = 0;
int   iHeight       = 0;
int   iHalfWidth    = 0;
int   iHalfHeight   = 0;
int   iFrameCounter = 0;
float iTimeDelta    = 0.0f;
float iTime         = 0.0f;

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
  
  clear();

  iWidth      = COLS * 2;
  iHeight     = LINES * 4;
  iHalfWidth  = iWidth / 2;
  iHalfHeight = iHeight / 2;

  timespec frame_time;
  timespec sleep_time;
  timespec remaining_time;
  timespec target_time = { 0, 16666666 }; 
  while (1)
  {
    timespec start_time;
    clock_gettime(CLOCK_REALTIME, &start_time);

    int ch;
    ch = getch();
    if (ch == KEY_LEFT)
    {
      offset_x -= 10;
    }
    if (ch == KEY_RIGHT)
    {
      offset_x += 10;
    }
    if (ch == KEY_UP)
    {
      offset_y -= 10;
    }
    if (ch == KEY_DOWN)
    {
      offset_y += 10;
    }

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


    mvprint_timespec( 0,0, "frame_time:      ", frame_time);
    mvprint_timespec( 1,0, "sleep_time:      ", sleep_time);
    mvprintw(2,0,          "iTimeDelta:      %f",iTimeDelta);
    mvprintw(3,0,          "iFrameCounter:   %d",iFrameCounter);
    mvprintw(4, 0,         "iResolution:     w:%d h:%d",iWidth,iHeight);

    uintptr_t bsu_ptr   = (uintptr_t)_binary_simulation_config_bsu_start;
    bsu*      bsu_start = (bsu*)bsu_ptr;
    mvprintw(5,0,          "magic:           %s", bsu_start->magic.utf8);

    uintptr_t size_ptr = (uintptr_t)_binary_simulation_config_bsu_size;
    uint32_t  size     = (uint32_t)size_ptr;
    mvprintw(6,0,          "size:            0x%08x", size);

    mvprintw(7,0,          "reference_count: %d", bsu_reference_count(bsu_start) );

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

  endwin();
  exit(0);
}

int mono_fragment( int u, int v )
{
  int result = 0;

  {
    int cx = iHalfWidth + offset_x;
    int cy = iHalfHeight + offset_y;
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
    float t  = iTime * 16.0f;
    float vx = cosf(t)+sinf(t);
    float vy = -sinf(t)+cosf(t);
    float dx = vx * 40.0f;
    float dy = vy * 40.0f;
    int   cx   = iHalfWidth + (int)dx;
    int   cy   = iHalfHeight + (int)dy;
    int   x    = u - cx;
    int   y    = v - cy;
    int   r    = 10;
    if ( (abs(x) <= r) && (abs(y) <= r) )
    {
      result |= sdCircle( x, y, r ) <= 0;
    }
  }

  result |= ((u%64)==0);
  result |= ((v%64)==0);

  return result;
}

// ---------------------------------------------------------------------------------------

void
write_age_location(bsu* bsu_start)
{
  uint32_t reference_count = bsu_reference_count(bsu_start);
  for (uint32_t reference_index=0;reference_index<reference_count;reference_index++)
  {

  }
}

