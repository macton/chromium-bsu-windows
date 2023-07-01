#pragma once

struct vec2
{
  float x;
  float y;

  vec2() = default;
  vec2(float a)
  {
    x = a;
    y = a;
  }
  vec2(float a, float b)
  {
    x = a;
    y = b;
  }
  vec2& operator-()
  {
      x = -x;
      y = -y;
      return *this;
  }
  vec2& operator+(vec2 b)
  {
      x = x + b.x;
      y = y + b.y;
      return *this;
  }
  vec2& operator-(vec2 b)
  {
      x = x - b.x;
      y = y - b.y;
      return *this;
  }
  vec2& operator+=(vec2 b)
  {
      x = x + b.x;
      y = y + b.y;
      return *this;
  }
  vec2& operator-=(vec2 b)
  {
      x = x - b.x;
      y = y - b.y;
      return *this;
  }
};

inline float min2(float a, float b)
{
    return (a < b) ? a : b;
}
inline float max2(float a, float b)
{
    return (a > b) ? a : b;
}

inline vec2 clamp(vec2 val, vec2 minval, vec2 maxval)
{
    val.x = min2(maxval.x, max2(minval.x, val.x));
    val.y = min2(maxval.y, max2(minval.y, val.y));
    return val;
}
