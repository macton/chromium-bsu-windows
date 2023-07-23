meta:
  id: bsu_file
  endian: le
seq:
  - id: magic
    type: str
    encoding: UTF-8
    size: 64
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
    type: static_array_u2
  asset_spawn:
    pos: _root.toc.asset_spawn_offset
    type: static_array_struct_spawn
  max_instance_count:
    pos: _root.toc.max_instance_count_offset
    type: static_array_u1
  instance_count:
    pos: _root.toc.instance_count_offset
    type: static_array_u1
  instance_location:
    pos: _root.toc.instance_location_offset
    type: static_array_struct_array_vec2
  instance_age:
    pos: _root.toc.instance_age_offset
    type: static_array_struct_array_u16
  pattern_u32:
    pos: _root.toc.pattern_u32_offset
    type: static_array_u4
  pattern_u8:
    pos: _root.toc.pattern_u8_offset
    type: static_array_u1
types:
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
  static_array_u2:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: u2
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
  static_array_struct_array_u16:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: struct_array_u16
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
  struct_vec2:
    seq:
      - id: x
        type: u2
      - id: 'y'
        type: u2
  struct_array_u16:
    seq:
      - id: value
        type: static_array_u2
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
      - id: target
        type: u2
      - id: time_step
        type: u2
      - id: offset
        type: struct_vec2
      - id: pattern_width
        type: u2
      - id: pattern_u32_index
        type: u2
  struct_at_group:
    seq:
      - id: target
        type: u2
      - id: time_step
        type: u2
      - id: time_start
        type: u2
      - id: time_stop
        type: u2
      - id: offset
        type: struct_vec2
      - id: pattern_count
        type: u2
      - id: pattern_u8_index
        type: u2
