#include <locale.h>
#include <curses.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

typedef struct timespec timespec;

int sdCircle( int x, int y, int r )
{
  return sqrt((x*x)+(y*y)) - r;
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
    int cx = iHalfWidth;
    int cy = iHalfHeight;
    int x  = u-cx;
    int y  = v-cy;
    int r  = 20 + abs((iFrameCounter % 80)-40);
    result |= sdCircle( x, y, r ) <= 0;
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
    result |= sdCircle( x, y, r ) <= 0;
  }

  result |= ((u%64)==0);
  result |= ((v%64)==0);

  return result;
}

