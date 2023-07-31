#pragma once
#include <math.h>
#include <float.h>

#define f32_max  fmaxf
#define f32_min  fminf
#define f32_sqrt sqrtf
#define f32_abs  fabsf

inline float f32_clamp( float a, float low, float high )
{
  return f32_max(f32_min(a,high),low);
}

inline float f32_sign( float a )
{
  return (a<0)?-1.0f:1.0f;
}

inline int f32_sd_circle( float x, float y, float r )
{
  return f32_sqrt((x*x)+(y*y)) - r;
}

inline float f32_sd_pentagon( float px, float py, float r )
{
  float kx = 0.809016994f;
  float ky = 0.587785252f;
  float kz = 0.726542528f;

  px  = f32_abs(px);

  float d0   = 2.0f * f32_min( (px*-kx)+(py*ky) , 0.0f );
  float k1x  = d0 * -kx;
  float k1y  = d0 * ky;

  px -= k1x;
  py -= k1y;

  float d1   = 2.0f * f32_min( (px*kx)+(py*ky) , 0.0f );
  float k3x  = d1 * kx;
  float k3y  = d1 * ky;

  px -= k3x;
  py -= k3y;

  float dx   = f32_clamp(px,-r*kz,r*kz);
  float dy   = r;

  px -= dx;
  py -= dy;

  return f32_sqrt( (px*px)+(py*py) ) * f32_sign(py);
}

inline float f32_sd_onion( float d, float r )
{
  return f32_abs(d)-r;
}

