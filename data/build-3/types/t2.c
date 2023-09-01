#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
  uint16_t sign = d >> 15;
  uint16_t m    = (d >> 10) & 0x1f;
  uint16_t f    = d & 0x3ff;
  uint32_t mag  = (m << 10 ) | f;
  float    f_mag = mag / 1024.0f;
  return (sign)?(-f_mag):f_mag;
}

void
print_dist( uint32_t d )
{
  float f = d_to_f(d);
  printf("| %12.6f | ", f);
  print_u32(d);
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
  uint32_t sign = rand() & 1;
  uint32_t m    = rand() & 0x1f;
  uint32_t f    = (rand() & 0xfff) << 3;
  return (sign << 31) | ( m << 15 ) | f;
}

uint32_t compress( uint32_t d )
{
  uint16_t f = (d >> 5) & 0x3ff;
  uint16_t m = (d >> 15) & 0x1f;
  uint16_t s = (d >> 31);
  return (s<<15) | (m << 10) | f;
}

int
main( void )
{
  srand(16552);
  for (int i=0;i<25;i++)
  {
    uint32_t d = rand_dist();
    print_dist(d);
  }
  printf("\n");
  printf("--------\n");
  printf("\n");
  srand(16552);
  for (int i=0;i<25;i++)
  {
    uint16_t d = compress(rand_dist());
      
    print_dist_16(d);
  }
  
  return 0;
}
