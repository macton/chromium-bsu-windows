#pragma once
#include <stdint.h>
#include "f32_math.h"

inline int32_t s32_abs( int32_t a )
{
  return (a<00)?-a:a;
}

inline int32_t s32_max( int32_t a, int32_t b )
{
  return ( a > b )?a:b;
}

inline int32_t s32_min( int32_t a, int32_t b )
{
  return ( a < b )?a:b;
}

inline int32_t s32_sqrt( int32_t a )
{
  return (int32_t)sqrtf( (float)a );
}

inline int32_t s32_clamp( int32_t a, int32_t low, int32_t high )
{
  return s32_max(s32_min(a,high),low);
}

inline int32_t s32_sign( int32_t a )
{
  return (a<0)?-1:1;
}

inline int s32_sd_circle( int32_t x, int32_t y, int32_t r )
{
  return (int32_t)f32_sd_circle( (float)x, (float)y, (float)r );
}

inline int32_t s32_sd_pentagon( int32_t px, int32_t py, int32_t r )
{
  return (int32_t)f32_sd_pentagon( (float)px, (float)py, (float)r );
}

inline int32_t s32_sd_onion( int32_t d, int32_t r )
{
  return (int32_t)f32_sd_onion( (float)d, (float)r );
}

