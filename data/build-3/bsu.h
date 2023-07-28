#pragma once
#include <stdint.h>

#define kAssetNamesOffset              64
#define kPlayAreaOffset                68
#define kPlayTimeOffset                72
#define kAssetBaseSizeOffset           76
#define kAssetBaseSpeedOffset          80
#define kAssetBaseHealthOffset         84
#define kAssetSpawnOffset              88
#define kMaxInstanceCountOffset        92
#define kInstanceCountOffset           96
#define kInstanceLocationOffset        100
#define kInstanceVelocityOffset        104
#define kInstanceAgeOffset             108
#define kInstanceHealthOffset          112
#define kPatternU32Offset              116
#define kPatternU8Offset               120
#define kCollisionModHealthOffset      124

typedef struct static_array         static_array;
typedef struct struct_vec2          struct_vec2;
typedef struct struct_play_area     struct_play_area;
typedef struct struct_play_time     struct_play_time;
typedef struct struct_spawn         struct_spawn;
typedef struct struct_at_each       struct_at_each;
typedef struct struct_at_group      struct_at_group;
typedef struct struct_collision_mod_health_target struct_collision_mod_health_target;
typedef struct struct_collision_mod_health struct_collision_mod_health;

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

struct struct_play_time
{
  float                time;
  float                time_delta;
  uint32_t             frame_counter;
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

struct struct_collision_mod_health_target
{
  uint32_t             target_asset_index;
  float                amount;
};

struct struct_collision_mod_health
{
  uint32_t             source_asset_index;
  static_array         targets;
};

