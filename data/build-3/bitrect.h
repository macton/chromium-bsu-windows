#pragma once
#include <stdint.h>
#include "f32_math.h"
#include "s32_math.h"

#ifndef __cplusplus
typedef struct bitrect_buffer bitrect_buffer;
#endif

struct bitrect_buffer
{
  uint32_t width;
  uint32_t height;
  uint8_t  buffer[0];
};

const uint8_t bitrect_bitmask[8] = { 1, 8, 2, 16, 4, 32, 64, 128 };

#define bitrect_calcsize(width,height)  ((((width)+1)/2) * (((height)+3)/4))
#define bitrect_calcstride(width)       (((width)+1)/2)

inline int32_t 
bitrect_size(bitrect_buffer* bitrect)
{
  return bitrect_calcsize(bitrect->width, bitrect->height);
}

inline int32_t 
bitrect_stride(bitrect_buffer* bitrect)
{
  return bitrect_calcstride(bitrect->width);
}

inline void
bitrect_clear(bitrect_buffer* bitrect)
{
  memset(bitrect->buffer,0,bitrect_size(bitrect));
}

inline uint8_t
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

void
bitrect_byte_write(bitrect_buffer* bitrect, int32_t x, int32_t y, uint8_t value)
{
  if ( x < 0 ) return;
  if ( y < 0 ) return;

  int32_t char_x      = x/2;
  int32_t char_y      = y/4;
  int32_t char_index  = (char_y * bitrect_stride(bitrect))+char_x;

  bitrect->buffer[ char_index ] = value;
}

uint8_t
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

void
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

void
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

void
bitrect_draw_circle(bitrect_buffer* bitrect, int32_t cx, int32_t cy, int32_t r)
{
  int32_t x0  = cx-r;
  int32_t y0  = cy-r;
  int32_t x1  = cx+r;
  int32_t y1  = cy+r;
  int32_t lcx = (x0+x1)/2;
  int32_t lcy = (y0+y1)/2;

  for (int32_t ly=y0;ly<=y1;ly++)
  {
    for (int32_t lx=x0;lx<=x1;lx++)
    {
      int32_t x = lx - lcx;
      int32_t y = ly - lcy;
      int32_t d = f32_sd_circle( x, y, r);
      if ( d <= 0 )
      {
        bitrect_bit_write( bitrect, lx, ly );
      }
    }
  }
}

