meta:
  id: bsu_file
  endian: le

seq:
  - id: magic
    type: name_string
  - id: toc
    type: toc

instances:
  by_reference:
    type: by_reference

types:
  name_string:
    seq:
      - id: utf8
        type: str
        encoding: UTF-8
        size: 64

  toc_offsets:
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

  toc:
    seq:
      - id: offsets
        type: toc_offsets
    instances:
      references:
        pos: offsets.reference_offset
        type: references
      playarea:
        pos: offsets.playarea_offset
        type: playarea
      location:
        pos: offsets.location_offset
        type: location
      base_size:
        pos: offsets.base_size_offset
        type: base_size
      base_speed:
        pos: offsets.base_speed_offset
        type: base_speed
      spawn:
        pos: offsets.spawn_offset
        type: spawn
      data_u8:
        pos: offsets.data_u8_offset
        type: data_u8
      data_u32:
        pos: offsets.data_u32_offset
        type: data_u32
      data_at_each:
        pos: offsets.data_at_each_offset
        type: data_at_each
      data_at_group:
        pos: offsets.data_at_group_offset
        type: data_at_group
      data_vec2:
        pos: offsets.data_vec2_offset
        type: data_vec2

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
      - id: count
        type: u4
      - id: value
        type: name_string
        repeat: expr
        repeat-expr: count

  location:
    seq:
      - id: value
        type: location_struct
        repeat: expr
        repeat-expr: _root.toc.references.count

  location_struct:
    seq:
      - id: data_vec2_offset
        type: u4 
      - id: max_count
        type: u4 
      - id: count
        type: u4 
    instances:
      value:
        type: location_by_index(_index, data_vec2_offset)
        repeat: expr
        repeat-expr: max_count

  base_size:
    seq:
      - id: value
        type: vec2
        repeat: expr
        repeat-expr: _root.toc.references.count

  base_speed:
    seq:
      - id: value
        type: f4
        repeat: expr
        repeat-expr: _root.toc.references.count

  spawn:
    seq:
      - id: value
        type: spawn_struct
        repeat: expr
        repeat-expr: _root.toc.references.count

  at_each_array:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: spawn_at_each_by_index(_index, offset)
        repeat: expr
        repeat-expr: count

  at_group_array:
    seq:
      - id: offset
        type: u4
      - id: count
        type: u4
    instances:
      value:
        type: spawn_at_group_by_index(_index, offset)
        repeat: expr
        repeat-expr: count
    
  spawn_struct:
    seq:
      - id: at_each
        type: at_each_array
      - id: at_group
        type: at_group_array

  data_u8:
    seq:
      - id: count
        type: u4
      - id: value
        type: u1
        repeat: expr
        repeat-expr: count

  data_u8_by_index:
    params:
      - id: index
        type: u4
      - id: u8_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_u8.value[u8_offset + index]

  data_u32:
    seq:
      - id: count
        type: u4
      - id: value
        type: u4
        repeat: expr
        repeat-expr: count

  data_u32_by_index:
    params:
      - id: index
        type: u4
      - id: u32_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_u32.value[u32_offset + index]

  pattern_u32:
    seq:
      - id: width
        type: u4
      - id: data_u32_offset
        type: u4
    instances:
      data_u32_count:
        value: (width+31)/32
      value:
        type: data_u32_by_index(_index, data_u32_offset)
        repeat: expr
        repeat-expr: data_u32_count

  data_at_each_struct:
    seq:
      - id: time_step
        type: f4
      - id: offset
        type: vec2
      - id: pattern
        type: pattern_u32

  data_at_each:
    seq:
      - id: count
        type: u4
      - id: value
        type: data_at_each_struct
        repeat: expr
        repeat-expr: _root.toc.data_at_each.count
  
  pattern_u8:
    seq:
      - id: count
        type: u4
      - id: data_u8_offset
        type: u4
    instances:
      value:
        type: data_u8_by_index(_index, data_u8_offset)
        repeat: expr
        repeat-expr: count

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
      - id: pattern
        type: pattern_u8

  data_at_group:
    seq:
      - id: count
        type: u4
      - id: value
        type: data_at_group_struct
        repeat: expr
        repeat-expr: count

  data_vec2:
    seq:
      - id: count
        type: u4
      - id: value
        type: vec2
        repeat: expr
        repeat-expr: count

  location_by_index:
    params:
      - id: index
        type: u4
      - id: data_vec2_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_vec2.value[data_vec2_offset + index]

  spawn_at_each_by_index:
    params:
      - id: index
        type: u4
      - id: data_at_each_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_at_each.value[data_at_each_offset + index]

  spawn_at_group_by_index:
    params:
      - id: index
        type: u4
      - id: data_at_group_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_at_group.value[data_at_group_offset + index]

  location_by_reference_index:
    params:
      - id: reference_index
        type: u4
    instances:
      count:
        value: _root.toc.location.value[reference_index].count
      max_count:
        value: _root.toc.location.value[reference_index].max_count
      data_vec2_offset:
        value: _root.toc.location.value[reference_index].data_vec2_offset
      value:
        type: location_by_index(_index, data_vec2_offset)
        repeat: expr
        repeat-expr: max_count

  by_reference_struct:
    params:
      - id: reference_index
        type: u4
    instances:
      name:
        value: _root.toc.references.value[reference_index]
      location:
        type: location_by_reference_index(reference_index)
      base_speed:
        value: _root.toc.base_speed.value[reference_index]
      base_size:
        value: _root.toc.base_size.value[reference_index]
      spawn_at_each:
        value: _root.toc.spawn.value[reference_index].at_each
      spawn_at_group:
        value: _root.toc.spawn.value[reference_index].at_group

  by_reference:
    instances:
      value:
        type: by_reference_struct(_index)
        repeat: expr
        repeat-expr: _root.toc.references.count
