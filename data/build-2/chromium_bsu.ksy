meta:
  id: bsu_file
  endian: le

seq:
  - id: magic
    type: name_string
  - id: toc
    type: toc

instances:
  reference_count:
    pos: toc.reference_offset
    type: u4
  references:
    pos: toc.reference_offset+4
    type: references
  playarea:
    pos: toc.playarea_offset
    type: playarea
  location:
    pos: toc.location_offset
    type: location
  base_size:
    pos: toc.base_size_offset
    type: base_size
  base_speed:
    pos: toc.base_speed_offset
    type: base_speed
  spawn:
    pos: toc.spawn_offset
    type: spawn
  data_u8_count:
    pos: toc.data_u8_offset
    type: u4
  data_u8:
    pos: toc.data_u8_offset+4
    type: data_u8
  data_u32_count:
    pos: toc.data_u32_offset
    type: u4
  data_u32:
    pos: toc.data_u32_offset+4
    type: data_u32
  data_at_each_count:
    pos: toc.data_at_each_offset
    type: u4
  data_at_each:
    pos: toc.data_at_each_offset+4
    type: data_at_each
  data_at_group_count:
    pos: toc.data_at_group_offset
    type: u4
  data_at_group:
    pos: toc.data_at_group_offset+4
    type: data_at_group
  data_vec2_count:
    pos: toc.data_vec2_offset
    type: u4
  data_vec2:
    pos: toc.data_vec2_offset+4
    type: data_vec2

  by_reference:
    type: by_reference

types:
  name_string:
    seq:
      - id: utf8
        type: str
        encoding: UTF-8
        size: 64
  toc:
    seq:
      - id: reference_offset
        type: u4
      - id: playarea_offset
        type: u4
      - id: location_offset
        type: u4
      - id: base_size_offset
        type: u4
      - id: base_speed_offset
        type: u4
      - id: spawn_offset
        type: u4
      - id: data_u8_offset
        type: u4
      - id: data_u32_offset
        type: u4
      - id: data_at_each_offset
        type: u4
      - id: data_at_group_offset
        type: u4
      - id: data_vec2_offset
        type: u4

  vec2:
    seq:
      - id: x
        type: f4
      - id: y
        type: f4

  playarea:
    seq:
      - id: size
        type: vec2

  references:
    seq:
      - id: value
        type: name_string
        repeat: expr
        repeat-expr: _root.reference_count

  location:
    seq:
      - id: value
        type: location_struct
        repeat: expr
        repeat-expr: _root.reference_count

  location_struct:
    seq:
      - id: vec2_offset
        type: u4 
      - id: max_count
        type: u4 
      - id: count
        type: u4 
    instances:
      value:
        type: location_by_index(_index, vec2_offset)
        repeat: expr
        repeat-expr: max_count
        if: max_count > 0

  base_size:
    seq:
      - id: value
        type: vec2
        repeat: expr
        repeat-expr: _root.reference_count

  base_speed:
    seq:
      - id: value
        type: f4
        repeat: expr
        repeat-expr: _root.reference_count

  spawn:
    seq:
      - id: value
        type: spawn_struct
        repeat: expr
        repeat-expr: _root.reference_count

  spawn_struct:
    seq:
      - id: at_each_offset
        type: u4
      - id: at_each_count
        type: u4
      - id: at_group_offset
        type: u4
      - id: at_group_count
        type: u4

  data_u8:
    seq:
      - id: value
        type: u1
        repeat: expr
        repeat-expr: _root.data_u8_count

  data_u8_by_index:
    params:
      - id: index
        type: s4
      - id: u8_offset
        type: s4
    instances:
      value:
        value: _root.data_u8.value[u8_offset + index]

  data_u32:
    seq:
      - id: value
        type: u4
        repeat: expr
        repeat-expr: _root.data_u32_count

  data_u32_by_index:
    params:
      - id: index
        type: s4
      - id: u32_offset
        type: s4
    instances:
      value:
        value: _root.data_u32.value[u32_offset + index]

  data_at_each_struct:
    seq:
      - id: time_step
        type: f4
      - id: offset
        type: vec2
      - id: pattern_width
        type: u4
      - id: pattern_data_u32_offset
        type: u4
    instances:
      pattern_data_u32_count:
        value: (pattern_width+31)/32
      pattern:
        type: data_u32_by_index(_index, pattern_data_u32_offset)
        repeat: expr
        repeat-expr: pattern_data_u32_count
        if: pattern_data_u32_count > 0

  data_at_each:
    seq:
      - id: value
        type: data_at_each_struct
        repeat: expr
        repeat-expr: _root.data_at_each_count

  data_at_group_struct:
    seq:
      - id: time_step
        type: f4
      - id: time_start
        type: f4
      - id: time_stop
        type: f4
      - id: offset
        type: vec2
      - id: pattern_data_u8_count
        type: u4
      - id: pattern_data_u8_offset
        type: u4
    instances:
      pattern:
        type: data_u8_by_index(_index, pattern_data_u8_offset)
        repeat: expr
        repeat-expr: pattern_data_u8_count
        if: pattern_data_u8_count > 0

  data_at_group:
    seq:
      - id: value
        type: data_at_group_struct
        repeat: expr
        repeat-expr: _root.data_at_group_count

  data_vec2:
    seq:
      - id: value
        type: vec2
        repeat: expr
        repeat-expr: _root.data_vec2_count

  location_by_index:
    params:
      - id: index
        type: s4
      - id: vec2_offset
        type: s4
    instances:
      value:
        value: _root.data_vec2.value[vec2_offset + index]

  spawn_at_each_by_index:
    params:
      - id: index
        type: s4
      - id: data_at_each_offset
        type: s4
    instances:
      value:
        value: _root.data_at_each.value[data_at_each_offset + index]

  spawn_at_group_by_index:
    params:
      - id: index
        type: s4
      - id: data_at_group_offset
        type: s4
    instances:
      value:
        value: _root.data_at_group.value[data_at_group_offset + index]

  by_reference_struct:
    params:
      - id: reference_index
        type: s4
    instances:
      name:
        value: _root.references.value[reference_index]
      location_count:
        value: _root.location.value[reference_index].count
      location_max_count:
        value: _root.location.value[reference_index].max_count
      location_vec2_offset:
        value: _root.location.value[reference_index].vec2_offset
        if: location_max_count > 0
      location:
        type: location_by_index(_index, location_vec2_offset)
        repeat: expr
        repeat-expr: location_max_count
        if: location_max_count > 0
      base_size:
        value: _root.base_size.value[reference_index]
      base_speed:
        value: _root.base_size.value[reference_index]
      spawn_at_each_count:
        value: _root.spawn.value[reference_index].at_each_count
      spawn_at_each_offset:
        value: _root.spawn.value[reference_index].at_each_offset
        if: spawn_at_each_count > 0
      spawn_at_each:
        type: spawn_at_each_by_index(_index, spawn_at_each_offset)
        repeat: expr
        repeat-expr: spawn_at_each_count
        if: spawn_at_each_count > 0
      spawn_at_group_count:
        value: _root.spawn.value[reference_index].at_group_count
      spawn_at_group_offset:
        value: _root.spawn.value[reference_index].at_group_offset
        if: spawn_at_group_count > 0
      spawn_at_group:
        type: spawn_at_group_by_index(_index, spawn_at_group_offset)
        repeat: expr
        repeat-expr: spawn_at_group_count
        if: spawn_at_group_count > 0

  by_reference:
    instances:
      value:
        type: by_reference_struct(_index)
        repeat: expr
        repeat-expr: _root.reference_count
