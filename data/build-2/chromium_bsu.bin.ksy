meta:
  id: bsu_file
  endian: le
seq:
  - id: magic
    type: str
    encoding: UTF-8
    size: 64
  - id: asset_names_offset
    type: u4
  - id: play_area_offset
    type: u4
  - id: asset_base_size_offset
    type: u4
  - id: asset_base_speed_offset
    type: u4
  - id: asset_spawn_offset
    type: u4
  - id: max_instance_count_offset
    type: u4
  - id: instance_count_offset
    type: u4
  - id: instance_location_offset
    type: u4
  - id: instance_age_offset
    type: u4
  - id: pattern_u32_offset
    type: u4
  - id: pattern_u8_offset
    type: u4
instances:
  asset_names:
    pos: asset_names_offset
    type: static_array_utf8
  play_area:
    pos: play_area_offset
    type: struct_play_area
  asset_base_size:
    pos: asset_base_size_offset
    type: static_array_struct_vec2
  asset_base_speed:
    pos: asset_base_speed_offset
    type: static_array_f4
  asset_spawn:
    pos: asset_spawn_offset
    type: static_array_struct_spawn
  max_instance_count:
    pos: max_instance_count_offset
    type: static_array_u4
  instance_count:
    pos: instance_count_offset
    type: static_array_u4
  instance_location:
    pos: instance_location_offset
    type: static_array_struct_array_vec2
  instance_age:
    pos: instance_age_offset
    type: static_array_struct_array_f32
  pattern_u32:
    pos: pattern_u32_offset
    type: static_array_u4
  pattern_u8:
    pos: pattern_u8_offset
    type: static_array_u1
types:
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
      - id: time_step
        type: f4
      - id: offset
        type: struct_vec2
      - id: pattern_width
        type: u4
      - id: pattern_u32_index
        type: u4
  struct_at_group:
    seq:
      - id: time_step
        type: f4
      - id: time_start
        type: f4
      - id: time_stop
        type: f4
      - id: offset
        type: struct_vec2
      - id: pattern_count
        type: u4
      - id: pattern_u8_index
        type: u4
