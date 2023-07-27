#pragma once
#include <stdint.h>

#define kAssetNamesOffset              64
#define kPlayAreaOffset                68
#define kAssetBaseSizeOffset           72
#define kAssetBaseSpeedOffset          76
#define kAssetBaseHealthOffset         80
#define kAssetSpawnOffset              84
#define kMaxInstanceCountOffset        88
#define kInstanceCountOffset           92
#define kInstanceLocationOffset        96
#define kInstanceVelocityOffset        100
#define kInstanceAgeOffset             104
#define kInstanceHealthOffset          108
#define kPatternU32Offset              112
#define kPatternU8Offset               116
#define kDistanceTrackerOffset         120

typedef struct static_array         static_array;
typedef struct struct_vec2          struct_vec2;
typedef struct struct_play_area     struct_play_area;
typedef struct struct_spawn         struct_spawn;
typedef struct struct_at_each       struct_at_each;
typedef struct struct_at_group      struct_at_group;
typedef struct struct_distance      struct_distance;
typedef struct struct_distance_tracker struct_distance_tracker;

#define kBsuInitialDirectionDown 0
#define kBsuInitialDirectionHero 1
#define kBsuInitialDirectionUp   2
#define kBsuOnFlagHeroTrigger0   1

struct static_array
{
  uint32_t             offset;
  uint32_t             count;
};

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
  uint32_t             target_index;
  float                time_step;
  float                time_next;
  struct_vec2          location_offset;
  uint32_t             pattern_width;
  uint32_t             pattern_u32_index;
};

struct struct_at_group
{
  uint32_t             target_index;
  float                time_step;
  float                time_start;
  float                time_stop;
  float                time_next;
  struct_vec2          location_offset;
  uint32_t             pattern_count;
  uint32_t             pattern_u8_index;
};

struct struct_distance
{
  uint32_t             target_asset_index;
  float                distance;
};

struct struct_distance_tracker
{
  uint32_t             source_asset_index;
  static_array         targets;
};

