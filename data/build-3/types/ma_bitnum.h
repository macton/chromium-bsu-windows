#pragma once

#include <stdint.h>

// ----------------------------------------------------------------------------------------
// Integer types
// Sizes: 16-bit, 32-bit
// Functions available per type:
//   - _li     // load immediate
//   - _dec    // decrement
//   - _inc    // increment
//   - _not    // bitwise complement
//   - _neg    // integer negate
//   - _ext    // sign-extend
//   - _and    // bitwise and
//   - _xor    // bitwise xor
//   - _andc   // bitwise and with complement
//   - _or     // bitwise or
//   - _srl    // shift right logical
//   - _sll    // shift left logical
//   - _add    // integer add
//   - _sub    // integer subtract
//   - _mul    // integer multiply
//   - _div    // integer divide
//   - _sels   // select on sign bit
//   - _selb   // select on bitwise mask
//   - _andsrl // bitwise and then shift right logical
//   - _srlm   // shift right logical then bitwise and with mask
//   - _addm   // integer add then bitwise and with mask
//   - _cntlz  // count leading zero
//   - _sra    // shift-right arithmetic
//   - _shift  // shift left (+) or right (-) logical 
//   - _shdiv  // left-shift numerator and integer divide
//   - _shmul  // integer multiply and right shift result
//
// Functions available for signed types:
//   - _tosm   // to signed-magnitude
//   - _fromsm // from signed-magnitude
//   - _abs    // absolute value
//
// Functions available for unsigned types:
//   - _addsm 
//   - _subsm

// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// 16 bit integer type
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// 32 bit integer type
// ----------------------------------------------------------------------------------------

// Load immediate
static inline uint32_t u32_li( uint32_t a )
{
  return (a);
}

// Decrement
static inline uint32_t u32_dec( uint32_t a )
{
  return (a - 1);
}

// Increment
static inline uint32_t u32_inc( uint32_t a )
{
  return (a + 1);
}

// Complement
static inline uint32_t u32_not( uint32_t a )
{
  return (~a);
}

// Negate
static inline uint32_t u32_neg( uint32_t a )
{
  return (-a);
}

// Extend sign
static inline uint32_t u32_ext( uint32_t a )
{
  return (((int32_t)a)>>31);
}

// And
static inline uint32_t u32_and( uint32_t a, uint32_t b )
{
  return (a & b);
}

// Exclusive Or
static inline uint32_t u32_xor( uint32_t a, uint32_t b )
{
  return (a ^ b);
}

// And with Complement
static inline uint32_t u32_andc( uint32_t a, uint32_t b )
{
  return (a & ~b);
}

// Or
static inline uint32_t u32_or( uint32_t a, uint32_t b )
{
  return (a | b);
}

// Shift Right Logical
static inline uint32_t u32_srl( uint32_t a, int sa )
{
  return (a >> sa);
}

// Shift Left Logical
static inline uint32_t u32_sll( uint32_t a, int sa )
{
  return (a << sa);
}

// Add
static inline uint32_t u32_add( uint32_t a, uint32_t b )
{
  return (a + b);
}

// Subtract
static inline uint32_t u32_sub( uint32_t a, uint32_t b )
{
  return (a - b);
}

// Multiply
static inline uint32_t u32_mul( uint32_t a, uint32_t b )
{
  return (a * b);
}

// Select on Sign bit
static inline uint32_t u32_sels( uint32_t test, uint32_t a, uint32_t b )
{
  const uint32_t mask   = u32_ext( test );
  const uint32_t sel_a  = u32_and(  a,     mask  );
  const uint32_t sel_b  = u32_andc( b,     mask  );
  const uint32_t result = u32_or(   sel_a, sel_b );

  return (result);
}

// Select Bits on mask
static inline uint32_t u32_selb( uint32_t mask, uint32_t a, uint32_t b )
{
  const uint32_t sel_a  = u32_and(  a,     mask  );
  const uint32_t sel_b  = u32_andc( b,     mask  );
  const uint32_t result = u32_or(   sel_a, sel_b );

  return (result);
}

// Load Immediate
static inline uint16_t u16_li( uint16_t a )
{
  return (a);
}

// Extend sign
static inline uint16_t u16_ext( uint16_t a )
{
  return (((int16_t)a)>>15);
}

// Negate
static inline uint16_t u16_neg( uint16_t a )
{
  return (-a);
}

// Complement
static inline uint16_t u16_not( uint16_t a )
{
  return (~a);
}

// Shift Left Logical
static inline uint16_t u16_sll( uint16_t a, int sa )
{
  return (a << sa);
}

// Shift Right Logical
static inline uint16_t u16_srl( uint16_t a, int sa )
{
  return (a >> sa);
}

// Add
static inline uint16_t u16_add( uint16_t a, uint16_t b )
{
  return (a + b);
}

// Increment
static inline uint16_t u16_inc( uint16_t a )
{
  return (a + 1);
}

// Decrement
static inline uint16_t u16_dec( uint16_t a )
{
  return (a - 1);
}

// Subtract
static inline uint16_t u16_sub( uint16_t a, uint16_t b )
{
  return (a - b);
}

// And
static inline uint16_t u16_and( uint16_t a, uint16_t b )
{
  return (a & b);
}

// Or
static inline uint16_t u16_or( uint16_t a, uint16_t b )
{
  return (a | b);
}

// Exclusive Or
static inline uint16_t u16_xor( uint16_t a, uint16_t b )
{
  return (a ^ b);
}

// And with Complement
static inline uint16_t u16_andc( uint16_t a, uint16_t b )
{
  return (a & ~b);
}

// And then Shift Right Logical
static inline uint16_t u16_andsrl( uint16_t a, uint16_t b, int sa )
{
  return ((a & b) >> sa);
}

// Shift Right Logical then Mask
static inline uint16_t u16_srlm( uint16_t a, int sa, uint16_t mask )
{
  return ((a >> sa) & mask);
}

// Add then Mask
static inline uint16_t u16_addm( uint16_t a, uint16_t b, uint16_t mask )
{
  return ((a + b) & mask);
}

// Select on Sign bit
static inline uint16_t u16_sels( uint16_t test, uint16_t a, uint16_t b )
{
  const uint16_t mask   = u16_ext( test );
  const uint16_t sel_a  = u16_and(  a,     mask  );
  const uint16_t sel_b  = u16_andc( b,     mask  );
  const uint16_t result = u16_or(   sel_a, sel_b );

  return (result);
}

// Count Leading Zeros
static inline uint32_t u32_cntlz( uint32_t x )
{
#ifdef __GNUC__
  /* NOTE: __builtin_clz is undefined for x == 0 */
  /* On PowerPC, this will map to insn: cntlzw   */
  /* On Pentium, this will map to insn: clz      */
  uint32_t is_x_nez_msb = u32_neg( x );
  uint32_t nlz          = __builtin_clz( x );
  uint32_t result       = u32_sels( is_x_nez_msb, nlz, 0x00000020 );
  return (result);
#else
  const uint32_t x0  = u32_srl(  x,  1 );
  const uint32_t x1  = u32_or(   x,  x0 );
  const uint32_t x2  = u32_srl(  x1, 2 );
  const uint32_t x3  = u32_or(   x1, x2 );
  const uint32_t x4  = u32_srl(  x3, 4 );
  const uint32_t x5  = u32_or(   x3, x4 );
  const uint32_t x6  = u32_srl(  x5, 8 );
  const uint32_t x7  = u32_or(   x5, x6 );
  const uint32_t x8  = u32_srl(  x7, 16 );
  const uint32_t x9  = u32_or(   x7, x8 );
  const uint32_t xA  = u32_not(  x9 );
  const uint32_t xB  = u32_srl(  xA, 1 );
  const uint32_t xC  = u32_and(  xB, 0x55555555 );
  const uint32_t xD  = u32_sub(  xA, xC );
  const uint32_t xE  = u32_and(  xD, 0x33333333 );
  const uint32_t xF  = u32_srl(  xD, 2 );
  const uint32_t x10 = u32_and(  xF, 0x33333333 );
  const uint32_t x11 = u32_add(  xE, x10 );
  const uint32_t x12 = u32_srl(  x11, 4 );
  const uint32_t x13 = u32_add(  x11, x12 );
  const uint32_t x14 = u32_and(  x13, 0x0f0f0f0f );
  const uint32_t x15 = u32_srl(  x14, 8 );
  const uint32_t x16 = u32_add(  x14, x15 );
  const uint32_t x17 = u32_srl(  x16, 16 );
  const uint32_t x18 = u32_add(  x16, x17 );
  const uint32_t x19 = u32_and(  x18, 0x0000003f );
  return ( x19 );
#endif
}

// Count Leading Zeros
static inline uint16_t u16_cntlz( uint16_t x )
{
#ifdef __GNUC__
  uint16_t nlz32 = (uint16_t)u32_cntlz( (uint32_t)x );
  uint32_t nlz   = u32_sub( nlz32, 16 );
  return (nlz);
#else
  const uint16_t x0  = u16_srl(  x,  1 );
  const uint16_t x1  = u16_or(   x,  x0 );
  const uint16_t x2  = u16_srl(  x1, 2 );
  const uint16_t x3  = u16_or(   x1, x2 );
  const uint16_t x4  = u16_srl(  x3, 4 );
  const uint16_t x5  = u16_or(   x3, x4 );
  const uint16_t x6  = u16_srl(  x5, 8 );
  const uint16_t x7  = u16_or(   x5, x6 );
  const uint16_t x8  = u16_not(  x7 );
  const uint16_t x9  = u16_srlm( x8, 1, 0x5555 );
  const uint16_t xA  = u16_sub(  x8, x9 );
  const uint16_t xB  = u16_and(  xA, 0x3333 );
  const uint16_t xC  = u16_srlm( xA, 2, 0x3333 );
  const uint16_t xD  = u16_add(  xB, xC );
  const uint16_t xE  = u16_srl(  xD, 4 );
  const uint16_t xF  = u16_addm( xD, xE, 0x0f0f );
  const uint16_t x10 = u16_srl(  xF, 8 );
  const uint16_t x11 = u16_addm( xF, x10, 0x001f );
  return ( x11 );
#endif
}

// Load Immediate
static inline int16_t s16_li( int16_t a )
{
  return (a);
}

// Extend sign
static inline int16_t s16_ext( int16_t a )
{
  return (((int16_t)a)>>15);
}

// Negate
static inline int16_t s16_neg( int16_t a )
{
  return (-a);
}

// Complement
static inline int16_t s16_not( int16_t a )
{
  return (~a);
}

// Decrement
static inline int16_t s16_dec( int16_t a )
{
  return (a - 1);
}

// Shift Left Logical
static inline int16_t s16_sll( int16_t a, int sa )
{
  return (a << sa);
}

// Shift Right Logical
static inline int16_t s16_srl( int16_t a, int sa )
{
  return ((uint16_t)a >> sa);
}

// Shift Right Arithmetic
static inline int16_t s16_sra( int16_t a, int sa )
{
  return (a >> sa);
}

// Add
static inline int16_t s16_add( int16_t a, int16_t b )
{
  return (a + b);
}

// Subtract
static inline int16_t s16_sub( int16_t a, int16_t b )
{
  return (a - b);
}

// And
static inline int16_t s16_and( int16_t a, int16_t b )
{
  return (a & b);
}

// Or
static inline int16_t s16_or( int16_t a, int16_t b )
{
  return (a | b);
}

// Exclusive Or
static inline int16_t s16_xor( int16_t a, int16_t b )
{
  return (a ^ b);
}

// And with Complement
static inline int16_t s16_andc( int16_t a, int16_t b )
{
  return (a & ~b);
}

// And then Shift Right Logical
static inline int16_t s16_andsrl( int16_t a, int16_t b, int sa )
{
  return ((a & b) >> sa);
}

// Shift Right Logical then Mask
static inline int16_t s16_srlm( int16_t a, int sa, int16_t mask )
{
  return ((a >> sa) & mask);
}

// Add then Mask
static inline int16_t s16_addm( int16_t a, int16_t b, int16_t mask )
{
  return ((a + b) & mask);
}

// Select on Sign bit
static inline int16_t s16_sels( int16_t test, int16_t a, int16_t b )
{
  const int16_t mask   = s16_ext( test );
  const int16_t sel_a  = s16_and(  a,     mask  );
  const int16_t sel_b  = s16_andc( b,     mask  );
  const int16_t result = s16_or(   sel_a, sel_b );

  return (result);
}

// Shift-multiply (a*b)>>sa
static inline int32_t s16_shmul( int16_t a, int16_t b, int16_t sa )
{
  int32_t a32 = a;
  int32_t b32 = b;
  int32_t c32 = (a * b) >> sa;
  return c32;
}

// Shift-divide (a<<sa)/b
static inline int16_t s16_shdiv( int16_t a, int16_t b, int16_t sa )
{
  int32_t a32 = a;
  int32_t b32 = b;
  int32_t c32 = (a << sa) / b;
  return c32;
}

uint16_t s16_to_signed_magnitude(int16_t d)
{
  uint16_t sign_mask          = u16_li( 0x8000 );
  uint16_t magnitude_mask     = u16_li( 0x7fff );
  uint16_t sign               = u16_and( d, sign_mask );
  uint16_t magnitude_unsigned = u16_and( d, magnitude_mask );
  uint16_t magnitude_not      = u16_not( magnitude_unsigned );
  uint16_t magnitude_signed   = u16_inc( magnitude_not );
  uint16_t magnitude          = u16_sels( sign, magnitude_signed, magnitude_unsigned );
  uint16_t signed_magnitude   = u16_or( sign, magnitude );
  return signed_magnitude;
}

int16_t s16_from_signed_magnitude(uint16_t d)
{
  uint16_t sign_mask          = u16_li( 0x8000 );
  uint16_t magnitude_mask     = u16_li( 0x7fff );
  uint16_t sign               = u16_and( d, sign_mask );
  uint16_t magnitude_unsigned = u16_and( d, magnitude_mask );
  uint16_t magnitude_dec      = u16_dec( magnitude_unsigned );
  uint16_t magnitude_signed   = u16_not( magnitude_dec );
  uint16_t magnitude          = u16_sels( sign, magnitude_signed, magnitude_unsigned );
  return (int16_t)magnitude;
}

/*
int32_t d32_to_signed(uint32_t d)
{
  uint32_t s_32 = d32_sign(d);
  uint32_t m_32 = d32_magnitude(d);
  return (uint32_t)(( s_32 ) ? ((~m_32)+1) : m_32);
}

uint32_t signed_to_d32(int32_t d)
{
  uint32_t s_32 = (d&0x80000000);
  uint32_t m_32 = s_32 ? (~(((uint32_t)d)-1)) : (uint32_t)d;
  return (uint32_t)s_32 | m_32;
}
*/


// Load Immediate
static inline int32_t s32_li( int32_t a )
{
  return (a);
}

// Extend sign
static inline int32_t s32_ext( int32_t a )
{
  return (((int32_t)a)>>31);
}

// Negate
static inline int32_t s32_neg( int32_t a )
{
  return (-a);
}

// Complement
static inline int32_t s32_not( int32_t a )
{
  return (~a);
}

// Decrement
static inline int32_t s32_dec( int32_t a )
{
  return (a - 1);
}

// Shift Left Logical
static inline int32_t s32_sll( int32_t a, int sa )
{
  return (a << sa);
}

// Shift Right Logical
static inline int32_t s32_srl( int32_t a, int sa )
{
  return ((uint32_t)a >> sa);
}

// Shift Right Arithmetic
static inline int32_t s32_sra( int32_t a, int sa )
{
  return (a >> sa);
}

// Add
static inline int32_t s32_add( int32_t a, int32_t b )
{
  return (a + b);
}

// Subtract
static inline int32_t s32_sub( int32_t a, int32_t b )
{
  return (a - b);
}

// And
static inline int32_t s32_and( int32_t a, int32_t b )
{
  return (a & b);
}

// Or
static inline int32_t s32_or( int32_t a, int32_t b )
{
  return (a | b);
}

// Exclusive Or
static inline int32_t s32_xor( int32_t a, int32_t b )
{
  return (a ^ b);
}

// And with Complement
static inline int32_t s32_andc( int32_t a, int32_t b )
{
  return (a & ~b);
}

// And then Shift Right Logical
static inline int32_t s32_andsrl( int32_t a, int32_t b, int sa )
{
  return ((a & b) >> sa);
}

// Shift Right Logical then Mask
static inline int32_t s32_srlm( int32_t a, int sa, uint32_t mask )
{
  return ((a >> sa) & mask);
}

// Add then Mask
static inline int32_t s32_addm( int32_t a, int32_t b, uint32_t mask )
{
  return ((a + b) & mask);
}

// Select on Sign bit
static inline int32_t s32_sels( int32_t test, int32_t a, int32_t b )
{
  const int32_t mask   = s32_ext( test );
  const int32_t sel_a  = s32_and(  a,     mask  );
  const int32_t sel_b  = s32_andc( b,     mask  );
  const int32_t result = s32_or(   sel_a, sel_b );
  return (result);
}

static inline int32_t s32_shmul( int32_t a, int32_t b, int32_t sa )
{
  int32_t a32 = a;
  int32_t b32 = b;
  int32_t c32 = (a * b) >> sa;
  return c32;
}

static inline int32_t s32_shdiv( int32_t a, int32_t b, int32_t sa )
{
  int32_t a32 = a;
  int32_t b32 = b;
  int32_t c32 = (a << sa) / b;
  return c32;
}

// ----------------------------------------------------------------------------------------
// Vector2 of 16bit type
// ----------------------------------------------------------------------------------------

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
