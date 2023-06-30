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
  vec2& operator+(vec2 b)
  {
      x = x + b.x;
      y = y + b.y;
      return *this;
  }
};
