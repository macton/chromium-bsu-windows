#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>

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
  printf("'");
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
  printf("'");
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
  printf("'");
  for (int i=15;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if ( i == 31 ) printf(":");
    if ( i == 10 ) printf(":");
  }
}

int16_t m_to_d2( float x )
{
	return (int16_t)(x * 1024.0f);
}  

int16_t d2_sqrt( int16_t d ) 
{
  float m = (float)d / 1024.0f;
  float m_sqrt = sqrtf( m );
  int16_t d_m_sqrt = m_to_d2( m_sqrt );

  return d_m_sqrt;
}

int32_t d2_32_sqrt( int32_t d ) 
{
  float m = (float)d / 1024.0f;
  float m_sqrt = sqrtf( m );
  int32_t d_m_sqrt = m_to_d2( m_sqrt );

  return d_m_sqrt;
}

int16_t d2_intpart(int16_t d)
{
	return (d >> 10);
}

int16_t d2_fractpart(int16_t d)
{
	return (d - (d2_intpart(d) << 10));
}

int32_t d2_32_intpart(int32_t d)
{
	return (d >> 10);
}

int32_t d2_32_fractpart(int32_t d)
{
	return (d - (d2_32_intpart(d) << 10));
}

int
print_fixed_1_row( float x )
{
  uint16_t d = (uint16_t)(x * 1024.0f);
  uint16_t m = d >> 10;
  uint16_t f = d & 0x3ff;
  float    ax = (float)m + (float)f/1024.0f;

  printf("'%-5.2f, %8d, 0x%04x, ",x,d,d);
  print_binary_6_10( d );
  printf(", '%4d, '%4d, '%4d + %4d/1024, '%12.8f", m, f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_fixed_2_row( float x )
{
  int16_t d = m_to_d2( x );
  int16_t m = d2_intpart( d );
  int16_t f = d2_fractpart( d );
  float   ax = (float)m + (float)f/1024.0f;
 
  printf("'%-5.2f, '%8d, 0x%04x, ",x,d,d&0x0000ffff);
  print_binary_u16( d );
  printf(", '%4d, '%4d, '%4d + %4d/1024, '%12.8f", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_d2_row( int16_t d )
{
  int16_t m = d2_intpart( d );
  int16_t f = d2_fractpart( d );
  float   ax = (float)m + (float)f/1024.0f;
 
  printf("'%8d, 0x%04x, ",d,d&0x0000ffff);
  print_binary_u16( d );
  printf(", '%4d, '%4d, '%4d + %4d/1024, '%12.8f", m,f,m,f,ax);
  printf("\n");

  return 0;
}

int
print_d2_32_row( int32_t d )
{
  int32_t m = d2_32_intpart( d );
  int32_t f = d2_32_fractpart( d );
  float   ax = (float)m + (float)f/1024.0f;
 
  printf("'%8d, 0x%08x, ",d,d);
  print_binary_u32( d );
  printf(", '%4d, '%4d, '%4d + %4d/1024, '%12.8f", m,f,m,f,ax);
  printf("\n");

  return 0;
}

uint16_t m_to_d3( float x )
{
  uint16_t s = (x<0.0f)?0x8000:0x0000;
  return (s|(int16_t)(fabsf(x) * 1024.0f));
}

uint16_t d3_signpart( uint16_t d )
{
  return (d >> 15)&0x01;
}

uint16_t d3_intpart( uint16_t d )
{
  return (d&0x7c00) >> 10;
}

uint16_t d3_fractpart( uint16_t d )
{
  return (d&0x3ff); 
}

uint32_t d3_32_signpart( uint32_t d )
{
  return (d >> 31)&0x01;
}

uint32_t d3_32_intpart( uint32_t d )
{
  return (d&0x7ffffc00) >> 10;
}

uint32_t d3_32_fractpart( uint32_t d )
{
  return (d&0x3ff); 
}

void
print_fixed_3_row( float x )
{
  uint16_t d = m_to_d3(x);
  uint16_t m = d3_intpart(d);
  uint16_t f = d3_fractpart(d);
  float   ax = ((float)m + (float)f/1024.0f) * (d3_signpart(d)?-1.0f:1.0f);
 
  printf("'%-5.2f, '%8d, 0x%04x, ",x,d,d&0x0000ffff);
  print_binary_1_5_10( d );
  printf(", '%1d, '%4d, '%4d, '%c(%2d + %4d/1024), '%12.8f", d3_signpart(d), m,f,(d3_signpart(d)?'-':' '),m,f,ax);
  printf("\n");
}

void
print_d3_32_row( int32_t d )
{
  uint16_t m = d3_32_intpart(d);
  uint16_t f = d3_32_fractpart(d);
  float   ax = ((float)m + (float)f/1024.0f) * (d3_32_signpart(d)?-1.0f:1.0f);
 
  printf("'%8d, 0x%08x, ",d,d);
  print_binary_1_21_10( d );
  printf(", '%1d, '%4d, '%4d, '%c(%2d + %4d/1024), '%12.8f", d3_32_signpart(d), m,f,(d3_32_signpart(d)?'-':' '),m,f,ax);
  printf("\n");
}

void
print_fixed_4_row( float x )
{
  uint16_t d  = (int16_t)((x+32.0f) * 1024.0f);
  uint16_t m  = (d >> 10);
  int16_t  sm = m-32;
  int16_t  f  = d & 0x3ff;
  float    ax = (float)sm + (float)f/1024.0f;
 
  printf("'%-5.2f, '%8d, 0x%04x, ",x,d,d&0x0000ffff);
  print_binary_u16( d );
  printf(", '%4d, '%4d, '%4d, '%4d + %4d/1024, '%12.8f", m, sm, f,m,f,ax);
  printf("\n");
}



int
main( void )
{
  printf("(x), (d), (d) hex, (d) binary 6:10, (m), (f), As fraction, Approx (x)\n");
  print_fixed_1_row( 1.0f );
  print_fixed_1_row( 1.2f );
  print_fixed_1_row( 1.5f );
  print_fixed_1_row( 2.0f );
  print_fixed_1_row( 15.0f );
  print_fixed_1_row( 25.5f );
  printf("\n");

  printf("(x), (d), (d) hex, (d) binary, (m), (f), As fraction, Approx (x)\n");
  print_fixed_2_row( 1.0f );
  print_fixed_2_row( -1.0f );
  print_fixed_2_row( 1.2f );
  print_fixed_2_row( -1.2f );
  print_fixed_2_row( 1.5f );
  print_fixed_2_row( -1.5f );
  print_fixed_2_row( 31.999f );
  print_fixed_2_row( -32.0f );
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
    int16_t  time_step       = m_to_d2( f_t );
    int16_t  speed           = m_to_d2( f_s );

    int16_t  in_p0_x         = m_to_d2( f_px0 );
    int16_t  in_p0_y         = m_to_d2( f_py0 );
    int16_t  in_p1_x         = m_to_d2( f_px1 );
    int16_t  in_p1_y         = m_to_d2( f_py1 );

    uint32_t p0              = ((uint32_t)(uint16_t)in_p0_x) | ((uint32_t)(uint16_t)in_p0_y << 16);
    uint32_t p1              = ((uint32_t)(uint16_t)in_p1_x) | ((uint32_t)(uint16_t)in_p1_y << 16);

    printf("in_p0_x: 0x%04x\n",in_p0_x);
    printf("in_p0_y: 0x%04x\n",in_p0_y);
    printf("in_p1_x: 0x%04x\n",in_p1_x);
    printf("in_p1_y: 0x%04x\n",in_p1_y);

    printf("p0: 0x%08x\n",p0);
    printf("p1: 0x%08x\n",p1);
 
    int32_t  p0_x       = (int32_t)((int16_t)p0);
    int32_t  p0_y       = (int32_t)((int16_t)(p0>>16));
    int32_t  p1_x       = (int32_t)((int16_t)p1);
    int32_t  p1_y       = (int32_t)((int16_t)(p1>>16));
    int32_t  speed_step = ((int32_t)speed * (int32_t)time_step)>>10; // speed_step = speed * time_step
    int32_t  v_x        = p1_x-p0_x;                                 // v  = p1-p0
    int32_t  v_y        = p1_y-p0_y;                                 //
    int32_t  v_x_2      = (v_x * v_x) >> 10;                         // v2 = (v*v)
    int32_t  v_y_2      = (v_y * v_y) >> 10;                         //
    int32_t  v_xy_2     = v_x_2 + v_y_2;                             //
    int32_t  v_len      = d2_32_sqrt( v_xy_2 );                      // v_len = sqrt( v2.x + v2.y )
    int32_t  nv_x       = (v_x << 10) / v_len;                       // nv = v / v_len
    int32_t  nv_y       = (v_y << 10) / v_len;                       //
    int32_t  d_x        = (nv_x * speed_step) >> 10;                 // d = nv * speed_step
    int32_t  d_y        = (nv_y * speed_step) >> 10;                 //
    int32_t  p2_x       = p0_x + d_x;                                // p2 = p0 + d
    int32_t  p2_y       = p0_y + d_y;                                //
    uint32_t result     = ((uint32_t)(uint16_t)p2_x) | ((uint32_t)(uint16_t)p2_y << 16);
   
    printf("time_step  ,"); print_d2_32_row(time_step);
    printf("speed      ,"); print_d2_32_row(speed);
    printf("speed_step ,"); print_d2_32_row(speed_step);
    printf("p0_x       ,"); print_d2_32_row(p0_x);
    printf("p0_y       ,"); print_d2_32_row(p0_y);
    printf("p1_x       ,"); print_d2_32_row(p1_x);
    printf("p1_y       ,"); print_d2_32_row(p1_y);
    printf("v_x        ,"); print_d2_32_row(v_x);
    printf("v_y        ,"); print_d2_32_row(v_y);
    printf("v_x_2      ,"); print_d2_32_row(v_x_2);
    printf("v_y_2      ,"); print_d2_32_row(v_y_2);
    printf("v_xy_2     ,"); print_d2_32_row(v_xy_2);
    printf("v_len      ,"); print_d2_32_row(v_len);
    printf("nv_x       ,"); print_d2_32_row(nv_x);
    printf("nv_y       ,"); print_d2_32_row(nv_y);
    printf("d_x        ,"); print_d2_32_row(d_x);
    printf("d_y        ,"); print_d2_32_row(d_y);
    printf("p2_x       ,"); print_d2_32_row(p2_x);
    printf("p2_y       ,"); print_d2_32_row(p2_y);
    printf("result     ,0x%08x\n",result);
    printf("\n");
  }

  printf("(x), (d), (d) hex, (d) binary, (s), (m), (f), As fraction, Approx (x)\n");
  print_fixed_3_row( 1.0f );
  print_fixed_3_row( -1.0f );
  print_fixed_3_row( 1.2f );
  print_fixed_3_row( -1.2f );
  print_fixed_3_row( 1.5f );
  print_fixed_3_row( -1.5f );
  printf("\n");

  {
    int16_t  time_step       = m_to_d3( f_t );
    int16_t  speed           = m_to_d3( f_s );

    int16_t  in_p0_x         = m_to_d3( f_px0 );
    int16_t  in_p0_y         = m_to_d3( f_py0 );
    int16_t  in_p1_x         = m_to_d3( f_px1 );
    int16_t  in_p1_y         = m_to_d3( f_py1 );

    uint32_t p0              = ((uint32_t)(uint16_t)in_p0_x) | ((uint32_t)(uint16_t)in_p0_y << 16);
    uint32_t p1              = ((uint32_t)(uint16_t)in_p1_x) | ((uint32_t)(uint16_t)in_p1_y << 16);

    printf("in_p0_x: 0x%04x\n",in_p0_x);
    printf("in_p0_y: 0x%04x\n",in_p0_y);
    printf("in_p1_x: 0x%04x\n",in_p1_x);
    printf("in_p1_y: 0x%04x\n",in_p1_y);

    printf("p0: 0x%08x\n",p0);
    printf("p1: 0x%08x\n",p1);
 
    int32_t  p0_x       = (int32_t)((int16_t)p0);
    int32_t  p0_y       = (int32_t)((int16_t)(p0>>16));
    int32_t  p1_x       = (int32_t)((int16_t)p1);
    int32_t  p1_y       = (int32_t)((int16_t)(p1>>16));
    int32_t  speed_step = ((int32_t)speed * (int32_t)time_step)>>10; // speed_step = speed * time_step

    int32_t  v_x        = p1_x-p0_x;                                 // v  = p1-p0
    int32_t  v_y        = p1_y-p0_y;                                 //

    int32_t  v_x_2      = (v_x * v_x) >> 10;                         // v2 = (v*v)
    int32_t  v_y_2      = (v_y * v_y) >> 10;                         //
    int32_t  v_xy_2     = v_x_2 + v_y_2;                             //
    int32_t  v_len      = d2_32_sqrt( v_xy_2 );                      // v_len = sqrt( v2.x + v2.y )
    int32_t  nv_x       = (v_x << 10) / v_len;                       // nv = v / v_len
    int32_t  nv_y       = (v_y << 10) / v_len;                       //
    int32_t  d_x        = (nv_x * speed_step) >> 10;                 // d = nv * speed_step
    int32_t  d_y        = (nv_y * speed_step) >> 10;                 //
    int32_t  p2_x       = p0_x + d_x;                                // p2 = p0 + d
    int32_t  p2_y       = p0_y + d_y;                                //
    uint32_t result     = ((uint32_t)(uint16_t)p2_x) | ((uint32_t)(uint16_t)p2_y << 16);
   
    printf("time_step  ,"); print_d3_32_row(time_step);
    printf("speed      ,"); print_d3_32_row(speed);
    printf("speed_step ,"); print_d3_32_row(speed_step);
    printf("p0_x       ,"); print_d3_32_row(p0_x);
    printf("p0_y       ,"); print_d3_32_row(p0_y);
    printf("p1_x       ,"); print_d3_32_row(p1_x);
    printf("p1_y       ,"); print_d3_32_row(p1_y);
    printf("v_x        ,"); print_d3_32_row(v_x);
    printf("v_y        ,"); print_d3_32_row(v_y);
    printf("v_x_2      ,"); print_d3_32_row(v_x_2);
    printf("v_y_2      ,"); print_d3_32_row(v_y_2);
    printf("v_xy_2     ,"); print_d3_32_row(v_xy_2);
    printf("v_len      ,"); print_d3_32_row(v_len);
    printf("nv_x       ,"); print_d3_32_row(nv_x);
    printf("nv_y       ,"); print_d3_32_row(nv_y);
    printf("d_x        ,"); print_d3_32_row(d_x);
    printf("d_y        ,"); print_d3_32_row(d_y);
    printf("p2_x       ,"); print_d3_32_row(p2_x);
    printf("p2_y       ,"); print_d3_32_row(p2_y);
    printf("result     ,0x%08x\n",result);
    printf("\n");
  }

  printf("(x), (d), (d) hex, (d) binary, (m), (sm), (f), As fraction, Approx (x)\n");
  print_fixed_4_row( 1.0f );
  print_fixed_4_row( -1.0f );
  print_fixed_4_row( 1.2f );
  print_fixed_4_row( -1.2f );
  print_fixed_4_row( 1.5f );
  print_fixed_4_row( -1.5f );
  printf("\n");

  int16_t min_s16 = SHRT_MIN;
  int16_t max_s16 = SHRT_MAX;

  printf("min_s16 %d %04x, %f m=%d\n",min_s16,min_s16, min_s16/1024.0f,min_s16>>10);
  printf("max_s16 %d %04x, %f m=%d\n",max_s16,max_s16, max_s16/1024.0f,max_s16>>10);

  return 0;
}
