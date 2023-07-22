#pragma once
#include <stdint.h>

#define kAssetNamesOffset              64
#define kPlayAreaOffset                68
#define kAssetBaseSizeOffset           72
#define kAssetBaseSpeedOffset          76
#define kAssetSpawnOffset              80
#define kMaxInstanceCountOffset        84
#define kInstanceCountOffset           88
#define kInstanceLocationOffset        92
#define kInstanceAgeOffset             96
#define kPatternU32Offset              100
#define kPatternU8Offset               104

typedef struct struct_vec2          struct_vec2;
typedef struct struct_play_area     struct_play_area;
typedef struct struct_spawn         struct_spawn;
typedef struct struct_at_each       struct_at_each;
typedef struct struct_at_group      struct_at_group;
typedef struct static_array         static_array;

struct struct_vec2
{
  float                x;
  float                y;
};

struct struct_play_area
{
  struct_vec2          size;
};

struct struct_spawn
{
  static_array         at_each;
  static_array         at_group;
};

struct struct_at_each
{
  float                time_step;
  struct_vec2          offset;
  uint32_t             pattern_width;
  uint32_t             pattern_u32_index;
};

struct struct_at_group
{
  float                time_step;
  float                time_start;
  float                time_stop;
  struct_vec2          offset;
  uint32_t             pattern_count;
  uint32_t             pattern_u8_index;
};

struct static_array
{
  uint32_t             offset;
  uint32_t             count;
};

