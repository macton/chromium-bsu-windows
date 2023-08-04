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

inline float f32_length( float x, float y )
{
  return sqrtf( (x*x)+(y*y) );
}

inline float f32_dot( float x0, float y0, float x1, float y1 )
{
  return (x0*x1)+(y0*y1);
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


inline float f32_sd_star5(float x, float y, float r, float rf)
{
    float k1_x =  0.809016994375f;
    float k1_y = -0.587785252292f;
    float k2_x = -k1_x;
    float k2_y =  k1_y;

    x  = f32_abs(x);
    float dk1 = f32_dot(k1_x,k1_y,x,y);
    x -= 2.0f*f32_max(dk1,0.0f)*k1_x;
    y -= 2.0f*f32_max(dk1,0.0f)*k1_y;

    float dk2 = f32_dot(k2_x,k2_y,x,y);
    x -= 2.0f*f32_max(dk2,0.0f)*k2_x;
    y -= 2.0f*f32_max(dk2,0.0f)*k2_y;

    x  = f32_abs(x);
    y -= r;
   
   float ba_x = rf * (-k1_y);
   float ba_y = rf * (k1_x) - 1.0f;

   float h = f32_clamp( f32_dot(x,y,ba_x,ba_y)/f32_dot(ba_x,ba_y,ba_x,ba_y), 0.0, r );

   return f32_length(x-ba_x*h, y-ba_y*h) * f32_sign(y*ba_x-x*ba_y);
}
