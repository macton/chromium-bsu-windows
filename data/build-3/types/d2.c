#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include "ma_bitnum.h"

int16_t f_to_d16( float x )
{
	return (int16_t)(x * 1024.0f);
}  

int16_t d32_sqrt( int32_t d ) 
{
  float m = (float)d / 1024.0f;
  float m_sqrt = sqrtf( m );
  return f_to_d16( m_sqrt );
}

int16_t d16_intpart(int16_t d)
{
  int16_t int_part = s16_sra(d, 10);
  return int_part;
}

int16_t d16_fractpart(int16_t d)
{
  uint16_t int_part_mask = u16_li(0xfc00);
  int16_t  int_part      = u16_and( d, int_part_mask );
  int16_t  fract_part    = s16_sub( d, int_part );
  return fract_part;
}

int32_t d32_intpart(int32_t d)
{
  int32_t int_part = s32_sra(d, 10);
  return int_part;
}

int32_t d32_fractpart(int32_t d)
{
	return (d - (d32_intpart(d) << 10));
}

int32_t d16_to_d32(int16_t d)
{
  return d;
}

int16_t d32_to_d16(int32_t d)
{
  return d;
}

int32_t d16_mul(int16_t a, int16_t b)
{
  int32_t c = s16_shmul( a, b, 10 );
  return c;
}

int16_t d16_div(int16_t a, int16_t b)
{
  int16_t c = s16_shdiv( a, b, 10 );
  return c;
}

int16_t d16_sub(int16_t a, int16_t b)
{
  int16_t c = s16_sub( a, b );
  return c;
}

int16_t d16_add(int16_t a, int16_t b)
{
  int16_t c = s16_add( a, b );
  return c;
}

int32_t d32_sub(int32_t a, int32_t b)
{
  int32_t c = s32_sub( a, b );
  return c;
}

int32_t d32_add(int32_t a, int32_t b)
{
  int32_t c = s32_add( a, b );
  return c;
}


void
print_binary_u16( uint16_t x ) 
{
  printf("'");
  for (int i=15;i>=0;i--)
  {
    uint16_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
  }
}

void
print_binary_u32( uint32_t x ) 
{
  printf("'");
  for (int i=31;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
  }
}

void
print_binary_6_10( uint16_t x ) 
{
  for (int i=15;i>=0;i--)
  {
    uint16_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if ( i == 10 ) printf(":");
  }
}

void
print_binary_1_5_10( uint16_t x ) 
{
  for (int i=15;i>=0;i--)
  {
    uint16_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if ( i == 15 ) printf(":");
    if ( i == 10 ) printf(":");
  }
}

void
print_binary_1_21_10( uint32_t x ) 
{
  for (int i=31;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if ( i == 31 ) printf(":");
    if ( i == 10 ) printf(":");
  }
}

int
print_row( float x )
{
  int16_t d = f_to_d16( x );
  int16_t m = d16_intpart( d );
  int16_t f = d16_fractpart( d );
  float   ax = (float)m + (float)f/1024.0f;
 
  printf("| %-5.2f | %4d | 0x%04x | ",x,d,d&0x0000ffff);
  print_binary_u16( d );
  printf("| %4d | %4d | %d + %d/1024 | %12.8f", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_d16_row( int16_t d )
{
  int16_t m = d16_intpart( d );
  int16_t f = d16_fractpart( d );
  float   ax = (float)m + (float)f/1024.0f;
 
  printf("| %4d | 0x%04x | ",d,d&0x0000ffff);
  print_binary_u16( d );
  printf("| %4d | %4d | %4d + %4d/1024 | %12.8f |", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_d32_row( int32_t d )
{
  int32_t m = d32_intpart( d );
  int32_t f = d32_fractpart( d );
  float   ax = (float)m + (float)f/1024.0f;
 
  printf("| %4d | 0x%08x | ",d,d);
  print_binary_u32( d );
  printf("| %4d | %4d | %4d + %4d/1024 | %12.8f |", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
main( void )
{
  printf("(x), (d), (d) hex, (d) binary, (m), (f), As fraction, Approx (x)\n");
  print_row( 1.0f );
  print_row( -1.0f );
  print_row( 1.2f );
  print_row( -1.2f );
  print_row( 1.5f );
  print_row( -1.5f );

  printf("| min | ");
  print_d16_row( 0x8000 );
  printf("| max | ");
  print_d16_row( 0x7fff );
  printf("\n");

  float f_px0 = 8.5f;
  float f_py0 = -4.25f;
  float f_px1 = -2.25f;
  float f_py1 = 4.7f;
  float f_t   = (1.0f/60.0f);
  float f_s   = 3.5f;
  printf("p0 (%f,%f) p1 (%f, %f) time_step (%f) speed (%f)\n",f_px0,f_py0, f_px1, f_py1, f_t, f_s );
  printf("\n");

  {
    // v = p1-p0
    float vx   = f_px1-f_px0;
    float vy   = f_py1-f_py0;

    // v2 = (v*v)
    float vx2  = vx * vx;
    float vy2  = vy * vy;

    // vlen = sqrt( v2.x + v2.y )
    float vxy2 = vx2+vy2;
    float vlen = sqrtf( vxy2 );

    // nv = v / vlen
    float nvx = vx / vlen;
    float nvy = vy / vlen;

    // speed_step = speed * time_step
    float speed_step = f_s * f_t;

    // d = nv * speed_step
    float dx   = nvx * speed_step;
    float dy   = nvy * speed_step;

    // p2 = p0 + d
    float px2  = f_px0 + dx;
    float py2  = f_py0 + dy;

    printf("vx         %f\n",vx);
    printf("vy         %f\n",vy);
    printf("vx2        %f\n",vx2);
    printf("vy2        %f\n",vy2);
    printf("vxy2       %f\n",vxy2);
    printf("vlen       %f\n",vlen);
    printf("nvx        %f\n",nvx);
    printf("nvy        %f\n",nvy);
    printf("speed_step %f\n",speed_step);
    printf("dx         %f\n",dx);
    printf("dy         %f\n",dy);
    printf("px2        %f\n",px2);
    printf("py2        %f\n",py2);
    printf("\n");
  }


  {
    int16_t  time_step       = f_to_d16( f_t );
    int16_t  speed           = f_to_d16( f_s );

    int16_t  in_p0_x         = f_to_d16( f_px0 );
    int16_t  in_p0_y         = f_to_d16( f_py0 );
    int16_t  in_p1_x         = f_to_d16( f_px1 );
    int16_t  in_p1_y         = f_to_d16( f_py1 );

    uint32_t p0              = ((uint32_t)(uint16_t)in_p0_x) | ((uint32_t)(uint16_t)in_p0_y << 16);
    uint32_t p1              = ((uint32_t)(uint16_t)in_p1_x) | ((uint32_t)(uint16_t)in_p1_y << 16);

    printf("in_p0_x: 0x%04x\n",in_p0_x);
    printf("in_p0_y: 0x%04x\n",in_p0_y);
    printf("in_p1_x: 0x%04x\n",in_p1_x);
    printf("in_p1_y: 0x%04x\n",in_p1_y);
    printf("\n");
 
    int16_t  p0_x          = d16_v2_x( p0 );              //
    int16_t  p0_y          = d16_v2_y( p0 );              //
    int16_t  p1_x          = d16_v2_x( p1 );              //
    int16_t  p1_y          = d16_v2_y( p1 );              //
    int32_t  speed_step_32 = d16_mul(speed,time_step);    // speed_step = speed * time_step
    int16_t  speed_step    = d32_to_d16(speed_step_32);   //
    int16_t  v_x           = d16_sub(p1_x,p0_x);          // v  = p1-p0
    int16_t  v_y           = d16_sub(p1_y,p0_y);          //
    int32_t  v_x_2         = d16_mul(v_x, v_x);           // v2 = (v*v)
    int32_t  v_y_2         = d16_mul(v_y, v_y);           //
    int32_t  v_xy_2        = d32_add(v_x_2,v_y_2);        //
    int16_t  v_len         = d32_sqrt( v_xy_2 );          // v_len = sqrt( v2.x + v2.y )
    int16_t  nv_x          = d16_div(v_x, v_len);         // nv = v / v_len
    int16_t  nv_y          = d16_div(v_y, v_len);         //
    int32_t  d_x_32        = d16_mul(nv_x, speed_step);   // d = nv * speed_step
    int16_t  d_x           = d32_to_d16(d_x_32);          //
    int32_t  d_y_32        = d16_mul(nv_y, speed_step);   //
    int16_t  d_y           = d32_to_d16(d_y_32);          //
    int16_t  p2_x          = d16_add(p0_x, d_x);          // p2 = p0 + d
    int16_t  p2_y          = d16_add(p0_y, d_y);          //
    uint32_t result        = d16_v2(p2_x,p2_y);           //
   
    printf("| p0            | 0x%08x\n",p0);
    printf("| p1            | 0x%08x\n",p1);
    printf("| time_step     "); print_d16_row(time_step);
    printf("| speed         "); print_d16_row(speed);
    printf("| p0_x          "); print_d16_row(p0_x);
    printf("| p0_y          "); print_d16_row(p0_y);
    printf("| p1_x          "); print_d16_row(p1_x);
    printf("| p1_y          "); print_d16_row(p1_y);
    printf("| speed_step_32 "); print_d32_row(speed_step_32);
    printf("| speed_step    "); print_d16_row(speed_step);
    printf("| v_x           "); print_d16_row(v_x);
    printf("| v_y           "); print_d16_row(v_y);
    printf("| v_x_2         "); print_d32_row(v_x_2);
    printf("| v_y_2         "); print_d32_row(v_y_2);
    printf("| v_xy_2        "); print_d32_row(v_xy_2);
    printf("| v_len         "); print_d16_row(v_len);
    printf("| nv_x          "); print_d16_row(nv_x);
    printf("| nv_y          "); print_d16_row(nv_y);
    printf("| d_x_32        "); print_d32_row(d_x_32);
    printf("| d_x           "); print_d16_row(d_x);
    printf("| d_y_32        "); print_d32_row(d_y_32);
    printf("| d_y           "); print_d16_row(d_y);
    printf("| p2_x          "); print_d16_row(p2_x);
    printf("| p2_y          "); print_d16_row(p2_y);
    printf("| result        ,0x%08x\n",result);
    printf("\n");
  }

  return 0;
}
