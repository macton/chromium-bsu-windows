#pragma once
#include <stdint.h>
#include "f32_math.h"
#include "s32_math.h"

#ifndef __cplusplus
typedef struct bitrect_buffer bitrect_buffer;
#endif

extern float iTime;

struct bitrect_buffer
{
  uint32_t width;
  uint32_t height;
  uint8_t  buffer[0];
};

const uint8_t bitrect_bitmask[8] = { 1, 8, 2, 16, 4, 32, 64, 128 };

#define bitrect_calcsize(width,height)  ((((width)+1)/2) * (((height)+3)/4))
#define bitrect_calcstride(width)       (((width)+1)/2)

extern inline int32_t 
bitrect_size(bitrect_buffer* bitrect)
{
  return bitrect_calcsize(bitrect->width, bitrect->height);
}

extern inline int32_t 
bitrect_stride(bitrect_buffer* bitrect)
{
  return bitrect_calcstride(bitrect->width);
}

extern inline void
bitrect_clear(bitrect_buffer* bitrect)
{
  memset(bitrect->buffer,0,bitrect_size(bitrect));
}

extern inline uint8_t
bitrect_byte_read(bitrect_buffer* bitrect, int32_t x, int32_t y)
{
  if ( x < 0 ) return 0;
  if ( y < 0 ) return 0;

  int32_t char_x      = x/2;
  int32_t char_y      = y/4;
  int32_t char_index  = (char_y * bitrect_stride(bitrect))+char_x;
  uint8_t char_value  = bitrect->buffer[ char_index ];

  return char_value;
}

extern inline void
bitrect_byte_write(bitrect_buffer* bitrect, int32_t x, int32_t y, uint8_t value)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;

  int32_t char_x      = x/2;
  int32_t char_y      = y/4;
  int32_t char_index  = (char_y * bitrect_stride(bitrect))+char_x;

  bitrect->buffer[ char_index ] = value;
}

extern inline uint8_t
bitrect_bit_read(bitrect_buffer* bitrect, int32_t x, int32_t y)
{
  if ( x < 0 ) return 0;
  if ( y < 0 ) return 0;

  int32_t char_x             = x/2;
  int32_t char_y             = y/4;
  int32_t char_index         = (char_y * bitrect_stride(bitrect))+char_x;
  uint8_t char_value         = bitrect->buffer[ char_index ];
  int32_t char_ox            = x - (char_x*2);
  int32_t char_oy            = y - (char_y*4);
  int32_t char_bitmask_index = (char_oy*2)+char_ox;
  uint8_t char_bitmask       = bitrect_bitmask[ char_bitmask_index ];

  return char_value & char_bitmask;
}

extern inline void
bitrect_bit_write(bitrect_buffer* bitrect, int32_t x, int32_t y)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;
  
  int32_t char_x             = x/2;
  int32_t char_y             = y/4;
  int32_t char_index         = (char_y * bitrect_stride(bitrect))+char_x;
  uint8_t char_value         = bitrect->buffer[ char_index ];
  int32_t char_ox            = x - (char_x*2);
  int32_t char_oy            = y - (char_y*4);
  int32_t char_bitmask_index = (char_oy*2)+char_ox;
  uint8_t char_bitmask       = bitrect_bitmask[ char_bitmask_index ];

  bitrect->buffer[ char_index ] |= char_bitmask;
}

extern inline void
bitrect_bit_clear(bitrect_buffer* bitrect, int32_t x, int32_t y)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;

  int32_t char_x             = x/2;
  int32_t char_y             = y/4;
  int32_t char_index         = (char_y * bitrect_stride(bitrect))+char_x;
  uint8_t char_value         = bitrect->buffer[ char_index ];
  int32_t char_ox            = x - (char_x*2);
  int32_t char_oy            = y - (char_y*4);
  int32_t char_bitmask_index = (char_oy*2)+char_ox;
  uint8_t char_bitmask       = bitrect_bitmask[ char_bitmask_index ];

  bitrect->buffer[ char_index ] &= ~char_bitmask;
}

extern inline void
bitrect_draw_circle(bitrect_buffer* bitrect, int32_t cx, int32_t cy, int32_t r)
{
  int32_t x0  = cx-r;
  int32_t y0  = cy-r;
  int32_t x1  = cx+r;
  int32_t y1  = cy+r;

  for (int32_t y=y0;y<=y1;y++)
  {
    for (int32_t x=x0;x<=x1;x++)
    {
      int32_t d = s32_sd_circle( x-cx, y-cy, r );
      if ( d <= 0 )
      {
        bitrect_bit_write( bitrect, x, y );
      }
    }
  }
}

extern inline void
bitrect_draw_star5(bitrect_buffer* bitrect, int32_t cx, int32_t cy, int32_t r)
{
  int32_t x0  = cx-(1*r);
  int32_t y0  = cy-(1*r);
  int32_t x1  = cx+(1*r);
  int32_t y1  = cy+(1*r);

  for (int32_t y=y0;y<=y1;y++)
  {
    for (int32_t x=x0;x<=x1;x++)
    {
      int32_t lx0 = x-cx;
      int32_t ly0 = y-cy;

      int32_t lx = (lx0*cosf(iTime*3.0))-(ly0*sinf(iTime*3.0)); 
      int32_t ly = (lx0*sinf(iTime*3.0))+(ly0*cosf(iTime*3.0));

      int32_t d = (int32_t)f32_sd_star5(lx, ly, r, 0.5f);
      if (d <= 0) 
      {
        bitrect_bit_write( bitrect, x, y );
      }
    }
  }
}

extern inline void
bitrect_draw_pentagon(bitrect_buffer* bitrect, int32_t cx, int32_t cy, int32_t r)
{
  int32_t x0  = cx-(1.25*r);
  int32_t y0  = cy-(1.25*r);
  int32_t x1  = cx+(1.25*r);
  int32_t y1  = cy+(1.25*r);

  for (int32_t y=y0;y<=y1;y++)
  {
    for (int32_t x=x0;x<=x1;x++)
    {
      int32_t d = s32_sd_pentagon( x-cx, y-cy, r );
      // int32_t d = (int32_t)f32_sd_star5(x-cx, y-cy, r, 0.5f);
      if (d <= 0) 
      {
        bitrect_bit_write( bitrect, x, y );
      }
    }
  }
}

extern inline void
bitrect_draw_circle_2(bitrect_buffer* bitrect, int32_t cx, int32_t cy, int32_t r)
{
  int32_t x0  = cx-r;
  int32_t y0  = cy-r;
  int32_t x1  = cx+r;
  int32_t y1  = cy+r;

  for (int32_t y=y0;y<=y1;y++)
  {
    for (int32_t x=x0;x<=x1;x++)
    {
      int32_t off_r = (int)(3.0 * sinf( iTime * 3.0f * atan2( y, x ) ));
      int32_t d = s32_sd_circle( x-cx, y-cy, r )-off_r;

/*
      if (( x == cx ) || ( y == cy ))
      {
        bitrect_bit_write( bitrect, x, y );
      }
      else if ( d <= 0 )
*/
      if ( d <= 0 )
      {
        int32_t dx = x-cx;
        int32_t dy = y-cy;
        int32_t xt = (dx * cosf( iTime * 3.f )) - (dy * sinf( iTime * 3.0f ));;
        int32_t yt = (dx * sinf( iTime * 3.f )) + (dy * cosf( iTime * 3.0f ));;
        int32_t enable_y = ((yt&1)==0);
        int32_t enable_x = ((xt&1)==0);
        int32_t enable = enable_x && enable_y;

        if (enable)
        bitrect_bit_write( bitrect, x, y );
      }
    }
  }
}

extern inline void
bitrect_draw_circle_border(bitrect_buffer* bitrect, int32_t cx, int32_t cy, int32_t circle_r, int32_t border_r)
{
  int32_t x0  = cx-(circle_r + border_r);
  int32_t y0  = cy-(circle_r + border_r);
  int32_t x1  = cx+(circle_r + border_r);
  int32_t y1  = cy+(circle_r + border_r);

  for (int32_t y=y0;y<=y1;y++)
  {
    for (int32_t x=x0;x<=x1;x++)
    {
      int32_t circle_d = s32_sd_circle( x-cx, y-cy, circle_r );
      int32_t border_d = s32_sd_onion( circle_d, border_r );
      if ( border_d <= 0 )
      {
        int32_t dx = x-cx;
        int32_t dy = y-cy;
        int32_t xt = (dx * cosf( iTime * 3.f )) - (dy * sinf( iTime * 3.0f ));;
        int32_t yt = (dx * sinf( iTime * 3.f )) + (dy * cosf( iTime * 3.0f ));;
        // int32_t enable_y = ((yt&3)==0) || ( xt == 0 );
        // int32_t enable_x = ((xt&3)==0) || ( yt == 0 );
        int32_t enable_y = ((yt&3)==0);
        int32_t enable_x = ((xt&3)==0);
        int32_t enable = enable_x && enable_y;
        if (enable)
        bitrect_bit_write( bitrect, x, y );
      }
    }
  }
}

