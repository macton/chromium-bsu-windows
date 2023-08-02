#pragma once
#include <stdint.h>

// Offset Map
#define kMutableOffset                 64
#define kMutableSize                   68
#define kAssetNamesOffset              72
#define kPlayAreaOffset                76
#define kAssetBaseSizeOffset           80
#define kAssetBaseSpeedOffset          84
#define kAssetBaseHealthOffset         88
#define kAssetSpawnOffset              92
#define kMaxInstanceCountOffset        96
#define kPatternU32Offset              100
#define kPatternU8Offset               104
#define kCollisionModHealthOffset      108
#define kCollisionModFlagsOffset       112
#define kPlayTimeOffset                116
#define kInstanceCountOffset           120
#define kInstanceVelocityOffset        124
#define kInstanceAgeOffset             128
#define kInstanceHealthOffset          132
#define kEventDestroyedAtOffset        136
#define kInstanceLocationOffset        140
#define kHeroFlagsOffset               144
#define kModFlagsOffset                148

// Named Constants
#define kEventDestroyedAtMaxCount      32
#define kInitialDirectionDown          0
#define kInitialDirectionHero          1
#define kInitialDirectionUp            2
#define kInitialDirectionOffset        3
#define kHeroTrigger0                  1
#define kHeroGun1                      2

#ifndef __cplusplus
typedef struct static_array                           static_array;
typedef struct struct_vec2                            struct_vec2;
typedef struct struct_play_area                       struct_play_area;
typedef struct struct_play_time                       struct_play_time;
typedef struct struct_spawn                           struct_spawn;
typedef struct struct_at_each                         struct_at_each;
typedef struct struct_at_group                        struct_at_group;
typedef struct struct_collision_mod_health_target     struct_collision_mod_health_target;
typedef struct struct_collision_mod_health            struct_collision_mod_health;
typedef struct struct_collision_mod_flags_target      struct_collision_mod_flags_target;
typedef struct struct_collision_mod_flags             struct_collision_mod_flags;
typedef struct struct_event_destroyed_at              struct_event_destroyed_at;
#endif // __cplusplus


struct static_array
{
  uint32_t    offset;
  uint32_t    count;
};

struct struct_vec2
{
  float  x;
  float  y;
};

struct struct_play_area
{
  struct_vec2  size;
};

struct struct_play_time
{
  float     time;
  float     time_delta;
  uint32_t  frame_counter;
};

struct struct_spawn
{
  static_array  at_each;
  static_array  at_group;
};

struct struct_at_each
{
  uint32_t     target_index;
  float        time_step;
  float        time_next;
  struct_vec2  location_offset;
  uint32_t     pattern_width;
  uint32_t     pattern_u32_index;
};

struct struct_at_group
{
  uint32_t     target_index;
  float        time_step;
  float        time_start;
  float        time_stop;
  float        time_next;
  struct_vec2  location_offset;
  uint32_t     pattern_count;
  uint32_t     pattern_u8_index;
};

struct struct_collision_mod_health_target
{
  uint32_t  target_asset_index;
  float     amount;
};

struct struct_collision_mod_health
{
  uint32_t      source_asset_index;
  static_array  targets;
};

struct struct_collision_mod_flags_target
{
  uint32_t  target_asset_index;
  uint32_t  value;
};

struct struct_collision_mod_flags
{
  uint32_t      source_asset_index;
  static_array  targets;
};

struct struct_event_destroyed_at
{
  uint32_t      count;
  static_array  at;
};

