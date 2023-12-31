meta:
  id: bsu_file
  endian: le
seq:
  - id: magic
    type: str
    encoding: UTF-8
    size: 64
  - id: mutable
    type: mutable
  - id: toc
    type: struct_toc
instances:
  asset_names:
    pos: _root.toc.asset_names_offset
    type: static_array_utf8
  play_area:
    pos: _root.toc.play_area_offset
    type: struct_play_area
  asset_base_size:
    pos: _root.toc.asset_base_size_offset
    type: static_array_struct_vec2
  asset_base_speed:
    pos: _root.toc.asset_base_speed_offset
    type: static_array_f4
  asset_base_health:
    pos: _root.toc.asset_base_health_offset
    type: static_array_f4
  asset_spawn:
    pos: _root.toc.asset_spawn_offset
    type: static_array_struct_spawn
  max_instance_count:
    pos: _root.toc.max_instance_count_offset
    type: static_array_u4
  pattern_u32:
    pos: _root.toc.pattern_u32_offset
    type: static_array_u4
  pattern_u8:
    pos: _root.toc.pattern_u8_offset
    type: static_array_u1
  collision_mod_health:
    pos: _root.toc.collision_mod_health_offset
    type: static_array_struct_collision_mod_health
  collision_mod_flags:
    pos: _root.toc.collision_mod_flags_offset
    type: static_array_struct_collision_mod_flags
  play_time:
    pos: _root.toc.play_time_offset
    type: struct_play_time
  instance_count:
    pos: _root.toc.instance_count_offset
    type: static_array_u4
  instance_velocity:
    pos: _root.toc.instance_velocity_offset
    type: static_array_struct_array_vec2
  instance_age:
    pos: _root.toc.instance_age_offset
    type: static_array_struct_array_f32
  instance_health:
    pos: _root.toc.instance_health_offset
    type: static_array_struct_array_f32
  event_destroyed_at:
    pos: _root.toc.event_destroyed_at_offset
    type: struct_event_destroyed_at
  instance_location:
    pos: _root.toc.instance_location_offset
    type: static_array_struct_array_vec2
  hero_flags:
    pos: _root.toc.hero_flags_offset
    type: u4
  mod_flags:
    pos: _root.toc.mod_flags_offset
    type: u4
types:
  mutable:
    seq:
      - id: offset
        type: u4
      - id: size
        type: u4
    instances:
      value:
        type: u1
        pos: offset
        repeat: expr
        repeat-expr: size
  struct_toc:
    seq:
      - id: asset_names_offset
        type: u4
      - id: play_area_offset
        type: u4
      - id: asset_base_size_offset
        type: u4
      - id: asset_base_speed_offset
        type: u4
      - id: asset_base_health_offset
        type: u4
      - id: asset_spawn_offset
        type: u4
      - id: max_instance_count_offset
        type: u4
      - id: pattern_u32_offset
        type: u4
      - id: pattern_u8_offset
        type: u4
      - id: collision_mod_health_offset
        type: u4
      - id: collision_mod_flags_offset
        type: u4
      - id: play_time_offset
        type: u4
      - id: instance_count_offset
        type: u4
      - id: instance_velocity_offset
        type: u4
      - id: instance_age_offset
        type: u4
      - id: instance_health_offset
        type: u4
      - id: event_destroyed_at_offset
        type: u4
      - id: instance_location_offset
        type: u4
      - id: hero_flags_offset
        type: u4
      - id: mod_flags_offset
        type: u4
  static_array_utf8:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: string_utf8
        pos: offset
        repeat: expr
        repeat-expr: count
  string_utf8:
    seq:
      - id: offset
        type: u4
    instances:
      value:
        type: str
        encoding: UTF-8
        pos: offset
        terminator: 0
  static_array_struct_vec2:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_vec2
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_f4:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: f4
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_struct_spawn:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_spawn
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_u4:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: u4
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_u1:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: u1
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_struct_collision_mod_health:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_collision_mod_health
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_struct_collision_mod_flags:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_collision_mod_flags
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_struct_array_vec2:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_array_vec2
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_struct_array_f32:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_array_f32
        pos: offset
        repeat: expr
        repeat-expr: count
  struct_vec2:
    seq:
      - id: x
        type: f4
      - id: 'y'
        type: f4
  struct_array_f32:
    seq:
      - id: value
        type: static_array_f4
  struct_array_vec2:
    seq:
      - id: value
        type: static_array_struct_vec2
  struct_play_area:
    seq:
      - id: size
        type: struct_vec2
  struct_play_time:
    seq:
      - id: time
        type: f4
      - id: time_delta
        type: f4
      - id: frame_counter
        type: u4
  static_array_struct_at_each:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_at_each
        pos: offset
        repeat: expr
        repeat-expr: count
  static_array_struct_at_group:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_at_group
        pos: offset
        repeat: expr
        repeat-expr: count
  struct_spawn:
    seq:
      - id: at_each
        type: static_array_struct_at_each
      - id: at_group
        type: static_array_struct_at_group
  struct_at_each:
    seq:
      - id: target_index
        type: u4
      - id: time_step
        type: f4
      - id: time_next
        type: f4
      - id: location_offset
        type: struct_vec2
      - id: pattern_width
        type: u4
      - id: pattern_u32_index
        type: u4
  struct_at_group:
    seq:
      - id: target_index
        type: u4
      - id: time_step
        type: f4
      - id: time_start
        type: f4
      - id: time_stop
        type: f4
      - id: time_next
        type: f4
      - id: location_offset
        type: struct_vec2
      - id: pattern_count
        type: u4
      - id: pattern_u8_index
        type: u4
  struct_collision_mod_health_target:
    seq:
      - id: target_asset_index
        type: u4
      - id: amount
        type: f4
  static_array_struct_collision_mod_health_target:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_collision_mod_health_target
        pos: offset
        repeat: expr
        repeat-expr: count
  struct_collision_mod_health:
    seq:
      - id: source_asset_index
        type: u4
      - id: targets
        type: static_array_struct_collision_mod_health_target
  struct_collision_mod_flags_target:
    seq:
      - id: target_asset_index
        type: u4
      - id: value
        type: u4
  static_array_struct_collision_mod_flags_target:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_collision_mod_flags_target
        pos: offset
        repeat: expr
        repeat-expr: count
  struct_collision_mod_flags:
    seq:
      - id: source_asset_index
        type: u4
      - id: targets
        type: static_array_struct_collision_mod_flags_target
  struct_event_destroyed_at:
    seq:
      - id: count
        type: u4
      - id: at
        type: static_array_struct_vec2
