#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

// ---------------------------

uint16_t f_to_d16( float x )
{
  return (uint16_t)((x+32.0f) * 1024.0f);
}

// ---------------------------

int16_t d32_sqrt( int32_t d ) 
{
  float m = ((float)d / 1024.0f)-32.0f;
  float m_sqrt = sqrtf( m );
  return f_to_d16( m_sqrt );
}

uint32_t d16_v2(uint16_t x, uint16_t y)
{
  return ((uint32_t)x) | ((uint32_t)y << 16);
}

uint16_t d16_v2_x(uint32_t a)
{
  return a;
}

uint16_t d16_v2_y(uint32_t a)
{
  return a >> 16;
}

// conversion and extraction

uint16_t d16_intpart( uint16_t d )
{
  return (d&0xfc00) >> 10;
}

uint16_t d16_fractpart( uint16_t d )
{
  return (d&0x3ff); 
}

uint32_t d32_intpart( uint32_t d )
{
  return (d&0xfffffc00) >> 10;
}

uint32_t d32_fractpart( uint32_t d )
{
  return (d&0x3ff); 
}

uint32_t d16_to_d32(uint16_t d)
{
  return d;
}

uint16_t d32_to_d16(uint32_t d)
{
  return d;
}

int32_t d32_to_signed(uint32_t d)
{
  return d-(32 << 10);
}

uint32_t signed_to_d32(int32_t s32)
{
  return s32+(32 << 10);
}

int16_t d16_to_signed(uint16_t d)
{
  return d-(32 << 10);
}

uint16_t signed_to_d16(int16_t s16)
{
  return s16+(32 << 10);
}

// math operations

uint32_t d16_mul(uint16_t a, uint16_t b)
{
  uint32_t a_32  = d16_to_d32( a );
  uint32_t b_32  = d16_to_d32( b );
  int32_t  a_s32 = d32_to_signed(a_32);
  int32_t  b_s32 = d32_to_signed(b_32);
  int32_t  c_s32 = ( a_s32 * b_s32 ) >> 10;
  uint32_t c_32  = signed_to_d32(c_s32);
  return c_32;
}

uint16_t d16_div(int16_t a, int16_t b)
{
  uint32_t a_32  = d16_to_d32( a );
  uint32_t b_32  = d16_to_d32( b );
  int32_t  a_s32 = d32_to_signed(a_32);
  int32_t  b_s32 = d32_to_signed(b_32);
  int32_t  c_s32 = (a_s32 << 10)/b_s32;
  uint32_t c_32  = signed_to_d32(c_s32);
  uint16_t c_16  = d32_to_d16( c_32 );
  return c_16;
}

uint16_t d16_sub(uint16_t a, uint16_t b)
{
  int16_t  a_s16 = d16_to_signed(a);
  int16_t  b_s16 = d16_to_signed(b);
  int16_t  c_s16 = a_s16-b_s16;
  uint16_t c     = signed_to_d16(c_s16);
  return c;
}

uint16_t d16_add(uint16_t a, uint16_t b)
{
  int16_t  a_s16 = d16_to_signed(a);
  int16_t  b_s16 = d16_to_signed(b);
  int16_t  c_s16 = a_s16+b_s16;
  uint16_t c     = signed_to_d16(c_s16);
  return c;
}

uint32_t d32_sub(uint32_t a, uint32_t b)
{
  int32_t  a_s32 = d32_to_signed(a);
  int32_t  b_s32 = d32_to_signed(b);
  int32_t  c_s32 = a_s32-b_s32;
  uint32_t c     = signed_to_d32(c_s32);
  return c;
}

uint32_t d32_add(uint32_t a, uint32_t b)
{
  int32_t  a_s32 = d32_to_signed(a);
  int32_t  b_s32 = d32_to_signed(b);
  int32_t  c_s32 = a_s32+b_s32;
  uint32_t c     = signed_to_d32(c_s32);
  return c;
}

void
print_binary_u16( uint16_t x ) 
{
  for (int i=15;i>=0;i--)
  {
    uint16_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
  }
  printf(" ");
}

void
print_binary_u32( uint32_t x ) 
{
  for (int i=31;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
  }
  printf(" ");
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
  printf(" ");
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
  printf(" ");
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
  printf(" ");
}

void
print_binary_22_10( uint32_t x ) 
{
  for (int i=31;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if ( i == 10 ) printf(":");
  }
  printf(" ");
}

int
print_row( float x )
{
  uint16_t d = f_to_d16( x );
  uint16_t m  = d16_intpart(d);
  uint16_t f  = d16_fractpart(d);
  int16_t  sm = (int16_t)m-32;
  float    ax = (float)sm + (float)f/1024.0f;
 
  printf("| %-5.2f | 0x%04x | ",x,d&0x0000ffff);
  print_binary_6_10( d );
  printf("| %4d | %4d | %4d + %4d/1024 | %12.8f |", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_d16_row( int16_t d )
{
  uint16_t m = d16_intpart(d);
  uint16_t f = d16_fractpart(d);
  int16_t  sm = (int16_t)m-32;
  float    ax = (float)sm + (float)f/1024.0f;
 
  printf("| 0x%04x | ",d&0x0000ffff);
  print_binary_6_10( d );
  printf("| %4d | %4d | %4d + %4d/1024 | %12.8f |", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_d32_row( uint32_t d )
{
  uint32_t m = d32_intpart(d);
  uint32_t f = d32_fractpart(d);
  int32_t  sm = (int32_t)m-32;
  float    ax = (float)sm + (float)f/1024.0f;
 
 
  printf("| 0x%08x | ",d);
  print_binary_22_10( d );
  printf("| %4d | %4d | %4d + %4d/1024 | %12.8f |", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
main( void )
{
  printf("(x), (d) hex, (d) binary, (m), (f), As fraction, Approx (x)\n");
  print_row( 1.0f );
  print_row( -1.0f );
  print_row( 1.2f );
  print_row( -1.2f );
  print_row( 1.5f );
  print_row( -1.5f );
  printf("| min  | ");
  print_d16_row( 0x0000 );
  printf("| max  | ");
  print_d16_row( 0xffff );
  printf("| zero | ");
  print_d16_row( 0x8000 );
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
 
    uint16_t  p0_x          = d16_v2_x( p0 );              //
    uint16_t  p0_y          = d16_v2_y( p0 );              //
    uint16_t  p1_x          = d16_v2_x( p1 );              //
    uint16_t  p1_y          = d16_v2_y( p1 );              //
    uint32_t  speed_step_32 = d16_mul(speed,time_step);    // speed_step = speed * time_step
    uint16_t  speed_step    = d32_to_d16(speed_step_32);   //
    uint16_t  v_x           = d16_sub(p1_x,p0_x);          // v  = p1-p0
    uint16_t  v_y           = d16_sub(p1_y,p0_y);          //
    uint32_t  v_x_2         = d16_mul(v_x, v_x);           // v2 = (v*v)
    uint32_t  v_y_2         = d16_mul(v_y, v_y);           //
    uint32_t  v_xy_2        = d32_add(v_x_2,v_y_2);        //
    uint16_t  v_len         = d32_sqrt( v_xy_2 );          // v_len = sqrt( v2.x + v2.y )
    uint16_t  nv_x          = d16_div(v_x, v_len);         // nv = v / v_len
    uint16_t  nv_y          = d16_div(v_y, v_len);         //
    uint32_t  d_x_32        = d16_mul(nv_x, speed_step);   // d = nv * speed_step
    uint16_t  d_x           = d32_to_d16(d_x_32);          //
    uint32_t  d_y_32        = d16_mul(nv_y, speed_step);   //
    uint16_t  d_y           = d32_to_d16(d_y_32);          //
    uint16_t  p2_x          = d16_add(p0_x, d_x);          // p2 = p0 + d
    uint16_t  p2_y          = d16_add(p0_y, d_y);          //
    uint32_t  result        = d16_v2(p2_x,p2_y);           //
   
    printf("p0            | 0x%08x\n",p0);
    printf("p1            | 0x%08x\n",p1);
    printf("time_step     "); print_d16_row(time_step);
    printf("speed         "); print_d16_row(speed);
    printf("p0_x          "); print_d16_row(p0_x);
    printf("p0_y          "); print_d16_row(p0_y);
    printf("p1_x          "); print_d16_row(p1_x);
    printf("p1_y          "); print_d16_row(p1_y);
    printf("speed_step_32 "); print_d32_row(speed_step_32);
    printf("speed_step    "); print_d16_row(speed_step);
    printf("v_x           "); print_d16_row(v_x);
    printf("v_y           "); print_d16_row(v_y);
    printf("v_x_2         "); print_d32_row(v_x_2);
    printf("v_y_2         "); print_d32_row(v_y_2);
    printf("v_xy_2        "); print_d32_row(v_xy_2);
    printf("v_len         "); print_d16_row(v_len);
    printf("nv_x          "); print_d16_row(nv_x);
    printf("nv_y          "); print_d16_row(nv_y);
    printf("d_x_32        "); print_d32_row(d_x_32);
    printf("d_x           "); print_d16_row(d_x);
    printf("d_y_32        "); print_d32_row(d_y_32);
    printf("d_y           "); print_d16_row(d_y);
    printf("p2_x          "); print_d16_row(p2_x);
    printf("p2_y          "); print_d16_row(p2_y);
    printf("result        | 0x%08x\n",result);
    printf("\n");
  }

  return 0;
}
