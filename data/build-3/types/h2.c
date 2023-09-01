#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <x86intrin.h>

// Load Immediate
static inline uint16_t _uint16_li( uint16_t a )
{
  return (a);
}

// Extend sign
static inline uint16_t _uint16_ext( uint16_t a )
{
  return (((int16_t)a)>>15);
}

// Negate
static inline uint16_t _uint16_neg( uint16_t a )
{
  return (-a);
}

// Complement
static inline uint16_t _uint16_not( uint16_t a )
{
  return (~a);
}

// Decrement
static inline uint16_t _uint16_dec( uint16_t a )
{
  return (a - 1);
}

// Shift Left Logical
static inline uint16_t _uint16_sll( uint16_t a, int sa )
{
  return (a << sa);
}

// Shift Right Logical
static inline uint16_t _uint16_srl( uint16_t a, int sa )
{
  return (a >> sa);
}

// Add
static inline uint16_t _uint16_add( uint16_t a, uint16_t b )
{
  return (a + b);
}

// Subtract
static inline uint16_t _uint16_sub( uint16_t a, uint16_t b )
{
  return (a - b);
}

// And
static inline uint16_t _uint16_and( uint16_t a, uint16_t b )
{
  return (a & b);
}

// Or
static inline uint16_t _uint16_or( uint16_t a, uint16_t b )
{
  return (a | b);
}

// Exclusive Or
static inline uint16_t _uint16_xor( uint16_t a, uint16_t b )
{
  return (a ^ b);
}

// And with Complement
static inline uint16_t _uint16_andc( uint16_t a, uint16_t b )
{
  return (a & ~b);
}

// And then Shift Right Logical
static inline uint16_t _uint16_andsrl( uint16_t a, uint16_t b, int sa )
{
  return ((a & b) >> sa);
}

// Shift Right Logical then Mask
static inline uint16_t _uint16_srlm( uint16_t a, int sa, uint16_t mask )
{
  return ((a >> sa) & mask);
}

// Add then Mask
static inline uint16_t _uint16_addm( uint16_t a, uint16_t b, uint16_t mask )
{
  return ((a + b) & mask);
}

static inline uint16_t _uint16_sels( uint16_t test, uint16_t a, uint16_t b )
{
  const uint16_t mask   = _uint16_ext( test );
  const uint16_t sel_a  = _uint16_and(  a,     mask  );
  const uint16_t sel_b  = _uint16_andc( b,     mask  );
  const uint16_t result = _uint16_or(   sel_a, sel_b );

  return (result);
}

// Select Bits on mask
static inline uint16_t _uint16_selb( uint16_t mask, uint16_t a, uint16_t b )
{
  const uint16_t sel_a  = _uint16_and(  a,     mask  );
  const uint16_t sel_b  = _uint16_andc( b,     mask  );
  const uint16_t result = _uint16_or(   sel_a, sel_b );

  return (result);
}

static inline uint16_t _uint16_cntlz( uint16_t x )
{
  /* NOTE: __builtin_clz is undefined for x == 0 */
  /* On PowerPC, this will map to insn: cntlzw   */
  /* On Pentium, this will map to insn: clz      */
  uint16_t is_x_nez_msb = _uint16_neg( x );
  uint16_t nlz          = __builtin_clz( (uint32_t)x )-16;
  uint16_t result       = _uint16_sels( is_x_nez_msb, nlz, 0x00000010 );
  return (result);
}

static inline uint16_t _uint16_sra( uint16_t a, uint16_t sa )
{
  return ((int16_t)a)>>sa;
}

static inline uint16_t _uint16_gt( uint16_t a, uint16_t b )
{
  uint16_t sub  = _uint16_sub(b,a);
  uint16_t mask = _uint16_ext(sub);
  return mask;
}

static inline uint16_t _uint16_lt( uint16_t a, uint16_t b )
{
  return _uint16_gt( b, a );
}

static inline uint16_t _uint16_abs( uint16_t a )
{
  return _uint16_sels(a, -a, a );
}

static inline uint16_t _uint16_shdiv( uint16_t a, uint16_t b, uint16_t sa )
{
  return ((uint32_t)a << sa) / b;
}

static inline uint16_t _uint16_shmul( uint16_t a, uint16_t b, uint16_t sa )
{
  return (((uint32_t)a * (uint32_t)b) >> 10);
}

static inline int16_t _int16_add( int16_t a, int16_t b )
{
  return a + b;
}

static inline int16_t _int16_sra( int16_t a, uint16_t sa )
{
  return a >> sa;
}

static inline int16_t _int16_sub( int16_t a, int16_t b )
{
  return a - b;
}

static inline uint16_t _int16_gt( int16_t a, int16_t b )
{
  int16_t  sub  = _int16_sub(b,a);
  uint16_t mask = _uint16_ext(sub);
  return mask;
}

static inline uint16_t _int16_lt( int16_t a, int16_t b )
{
  return _int16_gt(b,a);
}

static inline uint16_t _int16_ltz( int16_t a )
{
  return _int16_gt(0,a);
}

static inline uint16_t _int16_gtz( int16_t a )
{
  return _int16_gt(a,0);
}

// ---------------------------

uint16_t f_to_d16( float x )
{
  return _cvtss_sh(x, 0); 
}

float d16_to_f( uint16_t d )
{
  return _cvtsh_ss(d);
}

// ---------------------------

int16_t d16_sqrt( int32_t d ) 
{
  float m_sqrt = sqrtf( _cvtsh_ss(d) );
  return f_to_d16( m_sqrt );
}

uint32_t d16_v2(uint16_t x, uint16_t y)
{
  uint32_t lo = (uint32_t)x;
  uint32_t hi = ((uint32_t)y) << 16;
  return hi|lo;
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

uint16_t d16_sign( uint16_t d )
{
  return (d >> 15)&0x01;
}

uint16_t d16_exppart( uint16_t d )
{
  return (d&0x7c00) >> 10;
}

uint16_t d16_fractpart( uint16_t d )
{
  return (d&0x3ff); 
}

int16_t d16_exponent( uint16_t d )
{
  return d16_exppart(d)-15;
}

uint16_t d16_significand( uint16_t d )
{
  return d16_fractpart(d) | 0x400;
}

uint16_t d16_normalize( uint16_t sign, int16_t exponent, uint16_t significand )
{
  int16_t  norm_sa                = _uint16_cntlz( significand ) - 5;
  int16_t  norm_exponent          = _int16_sub( exponent, norm_sa );
  uint16_t significand_mask_r     = _int16_ltz( norm_sa );
  uint16_t significand_mask_l     = _int16_ltz( norm_sa );
  uint16_t significand_align_mask = _uint16_or( significand_mask_r, significand_mask_l );
  uint16_t significand_sr         = _uint16_srl( significand, -norm_sa );
  uint16_t significand_sl         = _uint16_sll( significand, norm_sa );
  uint16_t significand_align_r    = _uint16_and( significand_sr, significand_mask_r );
  uint16_t significand_align_l    = _uint16_and( significand_sl, significand_mask_l );
  uint16_t significand_align_sel  = _uint16_or( significand_align_r, significand_align_l );
  uint16_t significand_sel        = _uint16_selb( significand_align_mask, significand_align_sel, significand );
  uint16_t significand_part       = _uint16_and( significand_sel, 0x3ff );
  uint16_t sign_part              = _uint16_sll( sign, 15 );
  uint16_t exponent_biased        = _int16_add( norm_exponent, 15 );
  uint16_t exponent_part          = _uint16_sll( exponent_biased, 10 );
  uint16_t magnitude_part         = _uint16_or( exponent_part, significand_part );
  uint16_t c                      = _uint16_or( sign_part, magnitude_part );
  return c;
}

// math operations

uint32_t d16_mul(uint16_t a, uint16_t b)
{
  uint16_t a_sign        = d16_sign(a);
  int16_t  a_exponent    = d16_exponent(a);
  uint16_t a_significand = d16_significand(a);
  uint16_t b_sign        = d16_sign(b);
  int16_t  b_exponent    = d16_exponent(b);
  uint16_t b_significand = d16_significand(b);
  uint16_t c_sign        = _uint16_xor( a_sign, b_sign );
  int16_t  c_exponent    = _int16_add( a_exponent, b_exponent );
  uint16_t c_significand = _uint16_shmul( a_significand, b_significand, 10 );
  uint16_t c             = d16_normalize(c_sign, c_exponent, c_significand);
  return c;
}

uint16_t d16_div(int16_t a, int16_t b)
{
  uint16_t a_sign        = d16_sign(a);
  int16_t  a_exponent    = d16_exponent(a);
  uint16_t a_significand = d16_significand(a);
  uint16_t b_sign        = d16_sign(b);
  int16_t  b_exponent    = d16_exponent(b);
  uint16_t b_significand = d16_significand(b);
  uint16_t c_sign        = _uint16_xor( a_sign, b_sign );
  int16_t  c_exponent    = _int16_sub( a_exponent, b_exponent );
  uint16_t c_significand = _uint16_shdiv( a_significand, b_significand, 10 );
  uint16_t c             = d16_normalize( c_sign, c_exponent, c_significand );
  return c;
}

uint16_t d16_add(uint16_t a, uint16_t b)
{
  uint16_t a_sign                  = d16_sign(a);
  int16_t  a_exponent              = d16_exponent(a);
  uint16_t a_significand           = d16_significand(a);
  uint16_t b_sign                  = d16_sign(b);
  int16_t  b_exponent              = d16_exponent(b);
  uint16_t b_significand           = d16_significand(b);
  int16_t  c_exponent              = (a_exponent > b_exponent)?a_exponent:b_exponent;  

  // shift smaller exponent to align with larger
  uint16_t b_significand_sa_mask   = _int16_gt( a_exponent, b_exponent );
  uint16_t a_significand_sa_mask   = _int16_gt( b_exponent, a_exponent );
  uint16_t b_significand_sa_0      = _uint16_sub( a_exponent, b_exponent );
  uint16_t b_significand_sa        = _uint16_and( b_significand_sa_0, b_significand_sa_mask );
  uint16_t a_significand_sa_0      = _uint16_sub( b_exponent, a_exponent );
  uint16_t a_significand_sa        = _uint16_and( a_significand_sa_0, a_significand_sa_mask );
  uint16_t b_significand_aligned   = _uint16_srl( b_significand, b_significand_sa );
  uint16_t a_significand_aligned   = _uint16_srl( a_significand, a_significand_sa );

  // add aligned coefficients with appropriate signs
  uint16_t a_coeff                 = _uint16_sels( a, -a_significand_aligned, a_significand_aligned );
  uint16_t b_coeff                 = _uint16_sels( b, -b_significand_aligned, b_significand_aligned );
  int16_t  c_coeff                 = _int16_add( a_coeff, b_coeff );

  // extract sign and normalize result
  uint16_t c_sign                  = _uint16_ext( c_coeff );
  uint16_t c_significand           = _uint16_abs( c_coeff );
  uint16_t c                       = d16_normalize( c_sign, c_exponent, c_significand );

  return c;
}

uint16_t d16_sub(uint16_t a, uint16_t b)
{
  return d16_add( a, b^0x8000 );
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

int
print_row( float x )
{
  int16_t  d  = f_to_d16( x );
  uint16_t s  = d16_sign(d);
  int16_t  m  = d16_exponent( d );
  int16_t  f  = d16_fractpart( d );
  float    ax = d16_to_f( d );
  uint32_t base = 1024;
  if ( m < 0 )
  {
    base <<= -m;
  }
  else
  {
    base >>= m;
  }
 
  printf("| %-5.2f | 0x%04x | ",x,d&0x0000ffff);
  print_binary_1_5_10( d );
  printf("| %4d | %4d | %4d | %c(2^%4d) %c (%4d/%4d) | %12.8f |",s,m,f,s?'-':' ',m,s?'-':'+',f,base,ax);
  printf("\n");

  return 0;
}

int
print_d16_row( int16_t d )
{
  uint16_t s  = d16_sign(d);
  int16_t m  = d16_exponent(d);
  uint16_t f  = d16_fractpart(d);
  float    ax = d16_to_f( d );

  uint32_t base = 1024;
  if ( m < 0 )
  {
    base <<= -m;
  }
  else
  {
    base >>= m;
  }
 
  printf("| 0x%04x | ",d&0x0000ffff);
  print_binary_1_5_10( d );
  printf("| %4d | %4d | %4d | %c(2^%4d) %c (%4d/%4d) | %12.8f |", s,m,f,s?'-':' ',m,s?'-':'+',f,base,ax);
  printf("\n");

  return 0;
}

int
main( void )
{
  printf("(x), (d) hex, (d) binary, (s), (x), (f), As fraction, Approx (x)\n");
  print_row( 1.0f );
  print_row( -1.0f );
  print_row( 1.2f );
  print_row( -1.2f );
  print_row( 1.5f );
  print_row( -1.5f );
  printf("| -zero | ");
  print_d16_row( 0x8000 );
  printf("| +zero | ");
  print_d16_row( 0x0000 );
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
    uint16_t  speed_step    = d16_mul(speed,time_step);    // speed_step = speed * time_step
    uint16_t  v_x           = d16_sub(p1_x,p0_x);          // v  = p1-p0
    uint16_t  v_y           = d16_sub(p1_y,p0_y);          //
    uint16_t  v_x_2         = d16_mul(v_x, v_x);           // v2 = (v*v)
    uint16_t  v_y_2         = d16_mul(v_y, v_y);           //
    uint16_t  v_xy_2        = d16_add(v_x_2,v_y_2);        //
    uint16_t  v_len         = d16_sqrt( v_xy_2 );          // v_len = sqrt( v2.x + v2.y )
    uint16_t  nv_x          = d16_div(v_x, v_len);         // nv = v / v_len
    uint16_t  nv_y          = d16_div(v_y, v_len);         //
    uint16_t  d_x           = d16_mul(nv_x, speed_step);   // d = nv * speed_step
    uint16_t  d_y           = d16_mul(nv_y, speed_step);   //
    uint16_t  p2_x          = d16_add(p0_x, d_x);          // p2 = p0 + d
    uint16_t  p2_y          = d16_add(p0_y, d_y);          //
    uint32_t  result        = d16_v2(p2_x,p2_y);           //
   
    printf("| p0            | 0x%08x |\n",p0);
    printf("| p1            | 0x%08x |\n",p1);
    printf("| time_step     "); print_d16_row(time_step);
    printf("| speed         "); print_d16_row(speed);
    printf("| p0_x          "); print_d16_row(p0_x);
    printf("| p0_y          "); print_d16_row(p0_y);
    printf("| p1_x          "); print_d16_row(p1_x);
    printf("| p1_y          "); print_d16_row(p1_y);
    printf("| speed_step    "); print_d16_row(speed_step);
    printf("| v_x           "); print_d16_row(v_x);
    printf("| v_y           "); print_d16_row(v_y);
    printf("| v_x_2         "); print_d16_row(v_x_2);
    printf("| v_y_2         "); print_d16_row(v_y_2);
    printf("| v_xy_2        "); print_d16_row(v_xy_2);
    printf("| v_len         "); print_d16_row(v_len);
    printf("| nv_x          "); print_d16_row(nv_x);
    printf("| nv_y          "); print_d16_row(nv_y);
    printf("| d_x           "); print_d16_row(d_x);
    printf("| d_y           "); print_d16_row(d_y);
    printf("| p2_x          "); print_d16_row(p2_x);
    printf("| p2_y          "); print_d16_row(p2_y);
    printf("| result        | 0x%08x |\n",result);

    printf("\n");
  }

  return 0;
}
