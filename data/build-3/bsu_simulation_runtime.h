#pragma once

extern float debug_a;
extern float debug_b;
extern float debug_c;
extern float debug_d;

#define kHeroIndex       0
#define kHeroShieldIndex 1

extern inline void
bsu_simulation_write_play_time( uintptr_t bsu_start, float time, float time_delta, uint32_t frame_counter )
{
  struct_play_time* play_time = (struct_play_time*)(bsu_start + *(uint32_t*)(bsu_start + kPlayTimeOffset));

  play_time->time          = time;
  play_time->time_delta    = time_delta;
  play_time->frame_counter = frame_counter;
}

extern inline void
bsu_simulation_write_hero_flags( uintptr_t bsu_start, uint32_t flags )
{
  uint32_t* hero_flags = (uint32_t*)(bsu_start + *(uint32_t*)(bsu_start + kHeroFlagsOffset));
  *hero_flags = flags;
}

extern inline void
bsu_simulation_write_hero_location( uintptr_t bsu_start, float x, float y )
{
  static_array*  instance_location_array  = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*  instance_location_data   = (static_array*)(bsu_start + instance_location_array->offset);

  static_array*  hero_location_array      = instance_location_data + kHeroIndex;
  struct_vec2*   hero_location_data       = (struct_vec2*)(bsu_start + hero_location_array->offset);

  static_array*  hero_shield_location_array      = instance_location_data + kHeroShieldIndex;
  struct_vec2*   hero_shield_location_data       = (struct_vec2*)(bsu_start + hero_shield_location_array->offset);

  hero_location_data->x = x;
  hero_location_data->y = y;

  hero_shield_location_data->x = x;
  hero_shield_location_data->y = y;
}

extern inline void
bsu_simulation_enable_hero( uintptr_t bsu_start )
{
  static_array*  instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*      instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*  instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*  instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);

  static_array*  hero_age_array            = instance_age_data + kHeroIndex;
  float*         hero_age_data             = (float*)(bsu_start + hero_age_array->offset );

  static_array*  hero_shield_age_array     = instance_age_data + kHeroShieldIndex;
  float*         hero_shield_age_data      = (float*)(bsu_start + hero_shield_age_array->offset );

  instance_count_data[kHeroIndex] = 1;
  instance_count_data[kHeroShieldIndex] = 1;

  hero_age_data[0] = FLT_MIN;
  hero_shield_age_data[0] = FLT_MIN;
}

extern inline void
bsu_simulation_write_count_age_location_velocity( uintptr_t bsu_start )
{
  static_array*     asset_spawn_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetSpawnOffset));
  struct_spawn*     asset_spawn_data          = (struct_spawn*)(bsu_start + asset_spawn_array->offset);
  static_array*     asset_base_speed_array    = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSpeedOffset));
  float*            asset_base_speed_data     = (float*)(bsu_start + asset_base_speed_array->offset);
  static_array*     instance_location_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*     instance_location_data    = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*     instance_velocity_array   = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*     instance_velocity_data    = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*     instance_age_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*     instance_age_data         = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*     instance_health_array     = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceHealthOffset));
  static_array*     instance_health_data      = (static_array*)(bsu_start + instance_health_array->offset);
  static_array*     instance_count_array      = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*         instance_count_data       = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*     max_instance_count_array  = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*         max_instance_count_data   = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  static_array*     pattern_u8_array          = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kPatternU8Offset));
  uint8_t*          pattern_u8_data           = (uint8_t*)(bsu_start + pattern_u8_array->offset);
  static_array*     pattern_u32_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kPatternU32Offset));
  uint32_t*         pattern_u32_data          = (uint32_t*)(bsu_start + pattern_u32_array->offset);
  int               asset_count               = asset_spawn_array->count;
  struct_play_time* play_time                 = (struct_play_time*)(bsu_start + *(uint32_t*)(bsu_start + kPlayTimeOffset));
  float             t                         = play_time->time;
  static_array*     hero_location_array       = instance_location_data + kHeroIndex;
  struct_vec2*      hero_location_data        = (struct_vec2*)(bsu_start + hero_location_array->offset);
  float             hero_x                    = hero_location_data->x;
  float             hero_y                    = hero_location_data->y;
  uint32_t          hero_flags                = *(uint32_t*)(bsu_start + *(uint32_t*)(bsu_start + kHeroFlagsOffset));
  uint32_t          mod_flags                 = *(uint32_t*)(bsu_start + *(uint32_t*)(bsu_start + kModFlagsOffset));
  uint32_t          active_flags              = hero_flags | mod_flags;

  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    struct_spawn*     spawn               = asset_spawn_data + asset_index;
    static_array*     at_each_array       = &spawn->at_each;
    uint32_t          at_each_count       = at_each_array->count;
    struct_at_each*   at_each_data        = (struct_at_each*)(bsu_start + at_each_array->offset);
    static_array*     at_group_array      = &spawn->at_group;
    uint32_t          at_group_count      = at_group_array->count;
    struct_at_group*  at_group_data       = (struct_at_group*)(bsu_start + at_group_array->offset);
    float             base_speed          = asset_base_speed_data[ asset_index ];
    static_array*     location_array      = instance_location_data + asset_index;
    struct_vec2*      location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*     health_array        = instance_health_data + asset_index;
    float*            health_data         = (float*)(bsu_start + health_array->offset);
    static_array*     velocity_array      = instance_velocity_data + asset_index;
    struct_vec2*      velocity_data       = (struct_vec2*)(bsu_start + velocity_array->offset);
    static_array*     age_array           = instance_age_data + asset_index;
    float*            age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t          instance_count      = instance_count_data[asset_index];
    uint32_t          max_instance_count  = max_instance_count_data[asset_index];
    uint32_t          live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;
    struct_play_time* play_time           = (struct_play_time*)(bsu_start + *(uint32_t*)(bsu_start + kPlayTimeOffset));
    float             time_delta          = play_time->time_delta;

    // ---------------------------------------------------------------------------------------
    // Update instance location and age
    // ---------------------------------------------------------------------------------------

    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*       live_age       = age_data + live_instance_index;
      float*       live_health    = health_data + live_instance_index;
      struct_vec2* live_location  = location_data + live_instance_index;
      struct_vec2* live_velocity  = velocity_data + live_instance_index;

      if ( *live_age > 0 ) 
      {
        live_location->x += live_velocity->x * time_delta;
        live_location->y += live_velocity->y * time_delta;

        int out_band    = ((live_location->x < -17.0f) || (live_location->x > 17.0f) || (live_location->y < -17.0f) || (live_location->y > 17.0f));
        int out_health  = *live_health <= 0.0f;
        int end_of_life = (out_band || out_health) && (*live_age > FLT_MIN);
        *live_age = (end_of_life)?0.0f:(*live_age + time_delta);
      }
    }

    // ---------------------------------------------------------------------------------
    // at_each: Spawn an instance for each live instance of a target asset type
    // ---------------------------------------------------------------------------------

    for (int at_each_index=0;at_each_index<at_each_count;at_each_index++)
    {
      struct_at_each*  at_each                      = at_each_data + at_each_index;
      float            time_next                    = at_each->time_next;
      float            time_step                    = at_each->time_step;
      struct_vec2*     location_offset              = &at_each->location_offset;
      uint32_t         target_asset_index_direction = at_each->target_index;
      uint32_t         target_asset_index           = target_asset_index_direction >> 4;
      uint32_t         initial_direction            = target_asset_index_direction & 0x03;
      uint32_t         on_flag                      = (target_asset_index_direction >> 2) & 0x03;
      uint32_t         target_instance_count        = instance_count_data[target_asset_index];
      uint32_t         target_max_instance_count    = max_instance_count_data[target_asset_index];
      uint32_t         target_live_instance_count   = ( target_instance_count > target_max_instance_count ) ? target_max_instance_count : target_instance_count;
      static_array*    target_age_array             = instance_age_data + target_asset_index;
      float*           target_age_data              = (float*)(bsu_start + target_age_array->offset);

      if (( on_flag & active_flags ) != on_flag)
      {
        continue;
      }

      if (t >= time_next)
      {
        uint32_t       target_instance_count      = instance_count_data[target_asset_index];
        static_array*  target_location_array      = instance_location_data + target_asset_index;
        struct_vec2*   target_location_data       = (struct_vec2*)(bsu_start + target_location_array->offset);

        for (int target_instance_index=0;target_instance_index<target_live_instance_count;target_instance_index++)
        {
          float*         target_age                 = target_age_data + target_instance_index;

          if ( *target_age > 0.0f )
          {
            int            time_index                 = (int)((*target_age) / time_step);
            uint32_t       pattern_width              = at_each->pattern_width;
            uint32_t       pattern_index              = time_index % pattern_width;
            uint32_t       pattern_index_word_index   = pattern_index >> 5;
            uint32_t       pattern_index_bit_index    = pattern_index & 0x1f;
            uint32_t       pattern_word_start_index   = at_each->pattern_u32_index;
            uint32_t       pattern_word               = pattern_u32_data[ pattern_word_start_index + pattern_index_word_index ];
            uint32_t       pattern_bit                = pattern_word & ( 1 << pattern_index_bit_index );

            if ( pattern_bit )
            {
              struct_vec2*   target_location  = target_location_data + target_instance_index; 
              uint32_t       next_instance_index       = instance_count % max_instance_count; 
              struct_vec2*   next_instance_location    = location_data + next_instance_index;
              struct_vec2*   next_instance_velocity    = velocity_data + next_instance_index;
              float*         next_age                  = age_data + next_instance_index;
    
              next_instance_location->x = target_location->x + location_offset->x;
              next_instance_location->y = target_location->y + location_offset->y;
    
              float dir_x = 0.0f;
              float dir_y = 0.0f;
              switch (initial_direction)
              {
                case kInitialDirectionDown:
                  dir_y = -1.0f;
                break;
                case kInitialDirectionUp:
                  dir_y = 1.0f;
                break;
                case kInitialDirectionHero:
                {
                  float dx = hero_x - next_instance_location->x;
                  float dy = hero_y - next_instance_location->y;
                  float len = sqrtf( (dx*dx)+(dy*dy) );
                  dir_x = dx/len;
                  dir_y = dy/len;
                }
                break;
                case kInitialDirectionOffset:
                {
                  float dx = location_offset->x;
                  float dy = location_offset->y;
                  float len = sqrtf( (dx*dx)+(dy*dy) );
    
                  dir_x = dx/len;
                  dir_y = dy/len;
                }
                break;
              }
      
              next_instance_velocity->x = dir_x * base_speed;
              next_instance_velocity->y = dir_y * base_speed;
    
              *next_age = FLT_MIN;
              instance_count++;
            }
          }
        }

        at_each->time_next = t + time_step;
      }
    }

    // ---------------------------------------------------------------------------------
    // at_group: Spawn an instance given an instance index of a target asset type
    // ---------------------------------------------------------------------------------

    for (int at_group_index=0;at_group_index<at_group_count;at_group_index++)
    {
      struct_at_group* at_group                     = at_group_data + at_group_index;
      float            time_next                    = at_group->time_next;
      float            time_start                   = at_group->time_start;
      float            time_stop                    = at_group->time_stop;
      float            time_step                    = at_group->time_step;
      struct_vec2*     location_offset              = &at_group->location_offset;
      uint32_t         target_asset_index_direction = at_group->target_index;
      uint32_t         target_asset_index           = target_asset_index_direction >> 4;
      uint32_t         initial_direction            = target_asset_index_direction & 0x03;
      uint32_t         on_flag                      = (target_asset_index_direction >> 2) & 0x03;

      if (( on_flag & active_flags ) != on_flag )
      {
        continue;
      }

      if ((t >= time_next) && (t >= time_start) && (t <= time_stop))
      {
        int            time_index              = (int)((t-time_start) / time_step);
        uint32_t       target_instance_count   = instance_count_data[target_asset_index];
        static_array*  target_location_array   = instance_location_data + target_asset_index;
        struct_vec2*   target_location_data    = (struct_vec2*)(bsu_start + target_location_array->offset);
        uint32_t       pattern_count           = at_group->pattern_count;
        uint32_t       pattern_index           = at_group->pattern_u8_index + (time_index % pattern_count);
        uint32_t       target_instance_index   = pattern_u8_data[ pattern_index ];

        if ( target_instance_index < target_instance_count ) 
        {
          struct_vec2*   target_location  = target_location_data + target_instance_index;
          uint32_t       next_instance_index       = instance_count % max_instance_count;
          struct_vec2*   next_instance_location    = location_data + next_instance_index;
          struct_vec2*   next_instance_velocity    = velocity_data + next_instance_index;
          float*         next_age                  = age_data + next_instance_index;

          next_instance_location->x = target_location->x + location_offset->x;
          next_instance_location->y = target_location->y + location_offset->y;

          float dir_x = 0.0f;
          float dir_y = 0.0f;
          switch (initial_direction)
          {
            case kInitialDirectionDown:
              dir_y = -1.0f;
            break;
            case kInitialDirectionUp:
              dir_y = 1.0f;
            break;
            case kInitialDirectionHero:
            {
              float dx = hero_x - next_instance_location->x;
              float dy = hero_y - next_instance_location->y;
              float len = sqrtf( (dx*dx)+(dy*dy) );

              dir_x = dx/len;
              dir_y = dy/len;
            }
            break;
            case kInitialDirectionOffset:
            {
              float dx = location_offset->x;
              float dy = location_offset->y;
              float len = sqrtf( (dx*dx)+(dy*dy) );

              dir_x = dx/len;
              dir_y = dy/len;
            }
            break;
          }
  
          next_instance_velocity->x = dir_x * base_speed;
          next_instance_velocity->y = dir_y * base_speed;

          *next_age = FLT_MIN;
          instance_count++;
        }

        at_group->time_next = t + time_step;
      }
    }

    // ---------------------------------------------------------------------------------
    // write instance count
    // ---------------------------------------------------------------------------------

    instance_count_data[asset_index] = instance_count;
  }

}

extern inline void
bsu_simulation_write_health( uintptr_t bsu_start )
{
  static_array*                 asset_base_health_array           = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseHealthOffset));
  float*                        asset_base_health_data            = (float*)(bsu_start + asset_base_health_array->offset);
  static_array*                 asset_base_size_array             = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSizeOffset));
  struct_vec2*                  asset_base_size_data              = (struct_vec2*)(bsu_start + asset_base_size_array->offset);
  int                           asset_count                       = asset_base_health_array->count;
  static_array*                 instance_location_array           = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*                 instance_location_data            = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*                 instance_velocity_array           = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*                 instance_velocity_data            = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*                 instance_age_array                = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*                 instance_age_data                 = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*                 instance_count_array              = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*                     instance_count_data               = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*                 instance_health_array             = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceHealthOffset));
  static_array*                 instance_health_data              = (static_array*)(bsu_start + instance_health_array->offset);
  static_array*                 max_instance_count_array          = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*                     max_instance_count_data           = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  struct_play_time*             play_time                         = (struct_play_time*)(bsu_start + *(uint32_t*)(bsu_start + kPlayTimeOffset));
  float                         t                                 = play_time->time;
  static_array*                 collision_mod_health_array        = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kCollisionModHealthOffset));
  struct_collision_mod_health*  collision_mod_health_data         = (struct_collision_mod_health*)(bsu_start + collision_mod_health_array->offset);
  int                           collision_mod_health_source_count = collision_mod_health_array->count;
  struct_event_destroyed_at*    event_destroyed_at_data           = (struct_event_destroyed_at*)(bsu_start + *(uint32_t*)(bsu_start + kEventDestroyedAtOffset));
  uint32_t                      event_destroyed_at_count          = event_destroyed_at_data->count;
  static_array*                 event_destroyed_at_location_array = &event_destroyed_at_data->at;
  struct_vec2*                  event_destroyed_at_location_data  = (struct_vec2*)(bsu_start + event_destroyed_at_location_array->offset);

  // ---------------------------------------------------------------------------------
  // Write to health on collision (all instances of source vs all instances of target)
  // Write to event_destroyed_at
  // ---------------------------------------------------------------------------------

  for (int source_index=0;source_index<collision_mod_health_source_count;source_index++)
  {
    struct_collision_mod_health*         collision_mod_health       = collision_mod_health_data + source_index;
    uint32_t                             source_asset_index         = collision_mod_health->source_asset_index;
    uint32_t                             targets_offset             = collision_mod_health->targets.offset;
    uint32_t                             targets_count              = collision_mod_health->targets.count;
    struct_collision_mod_health_target*  targets_data               = (struct_collision_mod_health_target*)(bsu_start + targets_offset);
    static_array*                        source_age_array           = instance_age_data + source_asset_index;
    float*                               source_age_data            = (float*)(bsu_start + source_age_array->offset );
    struct_vec2                          source_base_size           = asset_base_size_data[ source_asset_index ];
    static_array*                        source_location_array      = instance_location_data + source_asset_index;
    struct_vec2*                         source_location_data       = (struct_vec2*)(bsu_start + source_location_array->offset);
    static_array*                        source_health_array        = instance_health_data + source_asset_index;
    float*                               source_health_data         = (float*)(bsu_start + source_health_array->offset);
    uint32_t                             source_instance_count      = instance_count_data[source_asset_index];
    uint32_t                             source_max_instance_count  = max_instance_count_data[source_asset_index];
    uint32_t                             source_live_instance_count = ( source_instance_count > source_max_instance_count ) ? source_max_instance_count : source_instance_count;

    for (int source_instance_index=0;source_instance_index<source_live_instance_count;source_instance_index++)
    {
      float*        source_age      = source_age_data + source_instance_index;
      struct_vec2*  source_location = source_location_data + source_instance_index;
      float*        source_health   = source_health_data + source_instance_index;

      if ( *source_age > 0)
      {
        for (int target_index=0;target_index<targets_count;target_index++)
        {
          struct_collision_mod_health_target*  target                     = targets_data + target_index;
          uint32_t                             target_asset_index         = target->target_asset_index;
          float                                target_mod_health_amount   = target->amount;
          static_array*                        target_age_array           = instance_age_data + target_asset_index;
          float*                               target_age_data            = (float*)(bsu_start + target_age_array->offset );
          struct_vec2                          target_base_size           = asset_base_size_data[ target_asset_index ];
          static_array*                        target_location_array      = instance_location_data + target_asset_index;
          struct_vec2*                         target_location_data       = (struct_vec2*)(bsu_start + target_location_array->offset);
          static_array*                        target_health_array        = instance_health_data + target_asset_index;
          float*                               target_health_data         = (float*)(bsu_start + target_health_array->offset);
          uint32_t                             target_instance_count      = instance_count_data[target_asset_index];
          uint32_t                             target_max_instance_count  = max_instance_count_data[target_asset_index];
          uint32_t                             target_live_instance_count = ( target_instance_count > target_max_instance_count ) ? target_max_instance_count : target_instance_count;
      
          for (int target_instance_index=0;target_instance_index<target_live_instance_count;target_instance_index++)
          {
            float*        target_age      = target_age_data + target_instance_index;
            struct_vec2*  target_location = target_location_data + target_instance_index;
            float*        target_health   = target_health_data + target_instance_index;
      
            if ( *target_age > 0)
            {
              float  r        = source_base_size.x + target_base_size.x;
              float  dx       = source_location->x - target_location->x;
              float  dy       = source_location->y - target_location->y;
              float  center_d = sqrtf( (dx*dx)+(dy*dy) );
              float  d        = center_d - r;
              if ( d <= 0.0f) 
              {
                *target_health += target_mod_health_amount;
                *source_health  = 0.0f; // self-destruct
                if ( ( *target_health <= 0.0f ) || ( *source_health <= 0.0f )) {
                  event_destroyed_at_location_data[event_destroyed_at_count % kEventDestroyedAtMaxCount] = *source_location;
                  event_destroyed_at_count++;
                }
              }
            }
          }
        }
      }
    }
  }

  event_destroyed_at_data->count = event_destroyed_at_count;

  // ---------------------------------------------------------------------------------
  // initialize instance health
  // ---------------------------------------------------------------------------------

  for (int asset_index=0;asset_index<asset_count;asset_index++)
  {
    float            base_health         = asset_base_health_data[ asset_index ];
    static_array*    health_array        = instance_health_data + asset_index;
    float*           health_data         = (float*)(bsu_start + health_array->offset);
    static_array*    location_array      = instance_location_data + asset_index;
    struct_vec2*     location_data       = (struct_vec2*)(bsu_start + location_array->offset);
    static_array*    velocity_array      = instance_velocity_data + asset_index;
    struct_vec2*     velocity_data       = (struct_vec2*)(bsu_start + velocity_array->offset);
    static_array*    age_array           = instance_age_data + asset_index;
    float*           age_data            = (float*)(bsu_start + age_array->offset );
    uint32_t         instance_count      = instance_count_data[asset_index];
    uint32_t         max_instance_count  = max_instance_count_data[asset_index];
    uint32_t         live_instance_count = ( instance_count > max_instance_count ) ? max_instance_count : instance_count;
   
    for (int live_instance_index=0;live_instance_index<live_instance_count;live_instance_index++)
    {
      float*  live_instance_age = age_data + live_instance_index;
      if ( *live_instance_age == FLT_MIN )
      {
        float* live_health = health_data + live_instance_index;
        *live_health = base_health;
      }
    }
  }
}

extern inline void
bsu_simulation_write_mod_flags( uintptr_t bsu_start )
{
  static_array*                 asset_base_size_array             = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kAssetBaseSizeOffset));
  struct_vec2*                  asset_base_size_data              = (struct_vec2*)(bsu_start + asset_base_size_array->offset);
  int                           asset_count                       = asset_base_size_array->count;
  static_array*                 instance_location_array           = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceLocationOffset));
  static_array*                 instance_location_data            = (static_array*)(bsu_start + instance_location_array->offset);
  static_array*                 instance_velocity_array           = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceVelocityOffset));
  static_array*                 instance_velocity_data            = (static_array*)(bsu_start + instance_velocity_array->offset);
  static_array*                 instance_age_array                = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceAgeOffset));
  static_array*                 instance_age_data                 = (static_array*)(bsu_start + instance_age_array->offset);
  static_array*                 instance_count_array              = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kInstanceCountOffset));
  uint32_t*                     instance_count_data               = (uint32_t*)(bsu_start + instance_count_array->offset);
  static_array*                 max_instance_count_array          = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kMaxInstanceCountOffset));
  uint32_t*                     max_instance_count_data           = (uint32_t*)(bsu_start + max_instance_count_array->offset);
  struct_play_time*             play_time                         = (struct_play_time*)(bsu_start + *(uint32_t*)(bsu_start + kPlayTimeOffset));
  float                         t                                 = play_time->time;
  static_array*                 collision_mod_flags_array         = (static_array*)(bsu_start + *(uint32_t*)(bsu_start + kCollisionModFlagsOffset));
  struct_collision_mod_flags*   collision_mod_flags_data          = (struct_collision_mod_flags*)(bsu_start + collision_mod_flags_array->offset);
  int                           collision_mod_flags_source_count  = collision_mod_flags_array->count;
  uint32_t*                     mod_flags                         = (uint32_t*)(bsu_start + *(uint32_t*)(bsu_start + kModFlagsOffset));

  // ---------------------------------------------------------------------------------
  // Write to flags on collision (all instances of source vs all instances of target)
  // ---------------------------------------------------------------------------------

  for (int source_index=0;source_index<collision_mod_flags_source_count;source_index++)
  {
    struct_collision_mod_flags*          collision_mod_flags        = collision_mod_flags_data + source_index;
    uint32_t                             source_asset_index         = collision_mod_flags->source_asset_index;
    uint32_t                             targets_offset             = collision_mod_flags->targets.offset;
    uint32_t                             targets_count              = collision_mod_flags->targets.count;
    struct_collision_mod_flags_target*   targets_data               = (struct_collision_mod_flags_target*)(bsu_start + targets_offset);
    static_array*                        source_age_array           = instance_age_data + source_asset_index;
    float*                               source_age_data            = (float*)(bsu_start + source_age_array->offset );
    struct_vec2                          source_base_size           = asset_base_size_data[ source_asset_index ];
    static_array*                        source_location_array      = instance_location_data + source_asset_index;
    struct_vec2*                         source_location_data       = (struct_vec2*)(bsu_start + source_location_array->offset);
    uint32_t                             source_instance_count      = instance_count_data[source_asset_index];
    uint32_t                             source_max_instance_count  = max_instance_count_data[source_asset_index];
    uint32_t                             source_live_instance_count = ( source_instance_count > source_max_instance_count ) ? source_max_instance_count : source_instance_count;


    for (int source_instance_index=0;source_instance_index<source_live_instance_count;source_instance_index++)
    {
      float*        source_age      = source_age_data + source_instance_index;
      struct_vec2*  source_location = source_location_data + source_instance_index;

      if ( *source_age > 0)
      {
        for (int target_index=0;target_index<targets_count;target_index++)
        {
          struct_collision_mod_flags_target*   target                     = targets_data + target_index;
          uint32_t                             target_asset_index         = target->target_asset_index;
          uint32_t                             target_mod_flags           = target->value;
          static_array*                        target_age_array           = instance_age_data + target_asset_index;
          float*                               target_age_data            = (float*)(bsu_start + target_age_array->offset );
          struct_vec2                          target_base_size           = asset_base_size_data[ target_asset_index ];
          static_array*                        target_location_array      = instance_location_data + target_asset_index;
          struct_vec2*                         target_location_data       = (struct_vec2*)(bsu_start + target_location_array->offset);
          uint32_t                             target_instance_count      = instance_count_data[target_asset_index];
          uint32_t                             target_max_instance_count  = max_instance_count_data[target_asset_index];
          uint32_t                             target_live_instance_count = ( target_instance_count > target_max_instance_count ) ? target_max_instance_count : target_instance_count;
      
          for (int target_instance_index=0;target_instance_index<target_live_instance_count;target_instance_index++)
          {
            float*        target_age      = target_age_data + target_instance_index;
            struct_vec2*  target_location = target_location_data + target_instance_index;
      
            if ( *target_age > 0)
            {
              float  r        = source_base_size.x + target_base_size.x;
              float  dx       = source_location->x - target_location->x;
              float  dy       = source_location->y - target_location->y;
              float  center_d = sqrtf( (dx*dx)+(dy*dy) );
              float  d        = center_d - r;
              if ( d <= 0.0f) 
              {
                *mod_flags |= target_mod_flags;
              }
            }
          }
        }
      }
    }
  }
}
