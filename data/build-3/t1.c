#include <stdint.h>
#include <stdio.h>

float
inv_lninterp( float a, float lo, float hi )
{
  uint32_t ulo = *(uint32_t*)&lo;
  uint32_t uhi = *(uint32_t*)&hi;
  uint32_t ua  = *(uint32_t*)&a;
  return (float)(ua-ulo)/(float)(uhi-ulo);
}

float
inv_lerp( float a, float lo, float hi )
{
  float c = (a-lo)/(hi-lo);
  return c;
}


int
main(void)
{
  float lo = 0.0f;
  float hi = 1.0f;
  float step = (hi-lo)/64.0f;
  float sub_step = step/64.0f;

  for (float a=lo;a<hi;a+=step)
  {
    float c = inv_lerp(a,lo,hi);
    printf("%f\n",c);
  }
  printf("-------\n");
  for (float a=lo;a<hi;a+=step)
  {
    float c = inv_lninterp(a,lo,hi);
    printf("%f\n",c);
  }
  printf("-------\n");

  return 0;
}
