#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "ma_bitnum.h"
#include <immintrin.h>

void
print_binary_u16( uint16_t x ) 
{
  for (int i=15;i>=0;i--)
  {
    uint16_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
  }
}

void
print_binary_u32( uint32_t x ) 
{
  for (int i=31;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
  }
}

void
print_binary_1_16_15( uint32_t x ) 
{
  for (int i=31;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if (i==31) printf(":");
    if (i==15) printf(":");
  }
}

void
print_binary_1_5_10( uint16_t x ) 
{
  for (int i=15;i>=0;i--)
  {
    uint32_t bit = 1 << i;
    printf("%c", ( x & bit )?'1':'0');
    if (i==15) printf(":");
    if (i==10) printf(":");
  }
}

void
print_value( int d )
{
  printf("%d, %d, 0x%04x, ",d,d,d);
  print_binary_u16(d) ;
  printf("\n");
}

void
print_u32( uint32_t d )
{
  uint32_t sign = d >> 31;
  uint32_t m    = d >> 15 & 0x0000ffff;
  uint32_t f    = d & 0x7fff;

  printf("%15u | 0x%08x | ",d,d);
  print_binary_1_16_15(d) ;
  // printf(" | %x | 0x%04x | 0x%04x |", sign, m, f);
  printf(" |\n");
}

void
print_u16( uint32_t d )
{
  uint32_t sign = d >> 15;
  uint32_t m    = (d >> 10) & 0x001f;
  uint32_t f    = (d >> 5) & 0x03ff; 

  printf("%8u |  0x%04x | ",d,d);
  print_binary_1_5_10(d) ;
  // printf(" | %x | 0x%04x | 0x%04x |", sign, m, f);
  printf(" |\n");
}

float
d_to_f( uint32_t d )
{
  uint32_t sign = d >> 31;
  uint32_t m    = d >> 15 & 0x0000ffff;
  uint32_t f    = d & 0x7fff;
  uint32_t mag  = (m << 15) | f;
  float    f_mag = mag / 32768.0f;
  return (sign)?(-f_mag):f_mag;
}

float
d16_to_f( uint16_t d )
{
  return _cvtsh_ss(d);
}

void
print_dist( uint32_t d )
{
  float f = d_to_f(d);
  printf("| %12.6f | ", f);
  print_u32(d);
}

void
print_dist_compare( uint32_t d )
{
  float f = d_to_f(d);
  printf("| %12.6f | ", f);
  print_binary_1_16_15(d);

  int clz = u32_cntlz(d);
  int clear_count = 32-clz-1-10;
  uint32_t clear_mask = ~((1 << clear_count)-1);
  uint32_t d_est = (d) & clear_mask;
  float f_est = d_to_f(d_est);

  printf(" -> ");

  print_binary_1_16_15(d_est);
  printf(" | ");
  printf("%12.6f |", f_est);
  printf("\n");
}

void
print_dist_16( uint32_t d )
{
  float f = d16_to_f(d);
  printf("| %12.6f | ", f);
  print_u16(d);
}

uint32_t
rand_dist()
{
  // uint32_t sign = rand() & 1;
  uint32_t sign = 0;
  uint32_t is_big = (rand() & 3) == 0;
  uint32_t m    = is_big?(rand() & 0x3ff):(1+(rand() & 0x1));
  uint32_t f    = is_big?((rand() & 0xfff) << 3):(rand() & 0x01ff);
  return (sign << 31) | ( m << 15 ) | f;
}

uint32_t
rand_zdist()
{
  // uint32_t sign = rand() & 1;
  uint32_t sign = 0;
  uint32_t m    = 0;
  uint32_t f    = (rand() & 0x7fff) >> (rand() & 0x0f);
  return (sign << 31) | ( m << 15 ) | f;
}

uint16_t compress( uint32_t d )
{
  uint32_t s   = d >> 31;
  int      clz = u32_cntlz(d&0x7fffffff)-1;
  int      exp = (30-clz);
  int      sa  = exp-15;

  // no possibility of getting sub-normal values from 1:16:15 format
  if ( clz > 29 ) 
  {
    exp = 0;
    sa  = 0;
  }

  uint32_t d10 = d >> 5; // align to 10 bit fraction
  uint32_t fraction = d10 & 0x3ff;
  if (sa > 0)
  {
    fraction = (d10 >> sa) & 0x3ff;
  }
  else if (sa < 0)
  {
    if (sa >= -4)
    {
      fraction = (d >> (5+sa)) & 0x3ff;
    }  
    else
    {
      fraction = d & (0x3ff >> -(sa+5));
    }
  }
  return (s<<15) | (exp << 10) | fraction;
}

// measure time
int
main( void )
{
  srand(16552);
  for (int i=0;i<23;i++)
  {
    uint32_t d = rand_dist();
    print_dist(d);
  }
  printf("\n");
  printf("--------\n");

  srand(16552);
  for (int i=0;i<23;i++)
  {
    uint32_t d = rand_dist();
    print_dist_compare(d);
  }
  printf("\n");
  printf("--------\n");

  printf("\n");
  srand(16552);
  for (int i=0;i<23;i++)
  {
    uint16_t d = compress(rand_dist());
    print_dist_16(d);
  }
  printf("\n");
  printf("--------\n");

  srand(16552);
  for (int i=0;i<23;i++)
  {
    uint32_t d = rand_zdist();
    print_dist(d);
  }
  printf("\n");
  printf("--------\n");

  printf("\n");
  srand(16552);
  for (int i=0;i<23;i++)
  {
    uint32_t d = rand_zdist();
    uint16_t d16 = compress(d);
    print_dist_16(d16);
  }
  printf("\n");
  printf("--------\n");
  
  return 0;
}
