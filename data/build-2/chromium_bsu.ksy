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
      - id: max_instance_count_offset
        type: u4
      - id: instance_count_offset
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
      - id: age_offset
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
      - id: data_f32_offset
        type: u4

  toc:
    seq:
      - id: offsets
        type: toc_offsets
    instances:
      references:
        pos: offsets.reference_offset
        type: references
      max_instance_count:
        pos: offsets.max_instance_count_offset
        type: max_instance_count
      instance_count:
        pos: offsets.instance_count_offset
        type: instance_count
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
      age:
        pos: offsets.age_offset
        type: age
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
      data_f32:
        pos: offsets.data_f32_offset
        type: data_f32

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

  max_instance_count:
    seq:
      - id: value
        type: u4
        repeat: expr
        repeat-expr: _root.toc.references.count

  instance_count:
    seq:
      - id: value
        type: u4
        repeat: expr
        repeat-expr: _root.toc.references.count

  location:
    seq:
      - id: value
        type: location_struct(_index)
        repeat: expr
        repeat-expr: _root.toc.references.count

  location_by_index:
    params:
      - id: index
        type: u4
      - id: data_vec2_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_vec2.value[data_vec2_offset + index]

  location_struct:
    params:
     - id: reference_index
       type: u4 
    seq:
      - id: data_vec2_offset
        type: u4 
    instances:
      max_count:
        value: _root.toc.max_instance_count.value[reference_index]
      value:
        type: location_by_index(_index, data_vec2_offset)
        repeat: expr
        repeat-expr: max_count

  location_by_reference_index:
    params:
      - id: reference_index
        type: u4
    instances:
      max_count:
        value: _root.toc.max_instance_count.value[reference_index]
      data_vec2_offset:
        value: _root.toc.location.value[reference_index].data_vec2_offset
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

  age:
    seq:
      - id: value
        type: age_struct(_index)
        repeat: expr
        repeat-expr: _root.toc.references.count

  age_by_index:
    params:
      - id: index
        type: u4
      - id: data_f32_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_f32.value[data_f32_offset + index]

  age_struct:
    params:
     - id: reference_index
       type: u4 
    seq:
      - id: data_f32_offset
        type: u4 
    instances:
      max_count:
        value: _root.toc.max_instance_count.value[reference_index]
      value:
        type: age_by_index(_index, data_f32_offset)
        repeat: expr
        repeat-expr: max_count

  age_by_reference_index:
    params:
      - id: reference_index
        type: u4
    instances:
      max_count:
        value: _root.toc.max_instance_count.value[reference_index]
      data_f32_offset:
        value: _root.toc.age.value[reference_index].data_f32_offset
      value:
        type: age_by_index(_index, data_f32_offset)
        repeat: expr
        repeat-expr: max_count

  spawn:
    seq:
      - id: value
        type: spawn_struct
        repeat: expr
        repeat-expr: _root.toc.references.count

  spawn_struct:
    seq:
      - id: at_each
        type: at_each_array
      - id: at_group
        type: at_group_array

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

  data_f32:
    seq:
      - id: count
        type: u4
      - id: value
        type: f4
        repeat: expr
        repeat-expr: count

  data_f32_by_index:
    params:
      - id: index
        type: u4
      - id: f32_offset
        type: u4
    instances:
      value:
        value: _root.toc.data_f32.value[f32_offset + index]

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
      - id: next_spawn_time
        type: f4
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
      - id: next_spawn_time
        type: f4
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

  by_reference_struct:
    params:
      - id: reference_index
        type: u4
    instances:
      name:
        value: _root.toc.references.value[reference_index]
      max_instance_count:
        value: _root.toc.max_instance_count.value[reference_index]
      instance_count:
        value: _root.toc.instance_count.value[reference_index]
      location:
        type: location_by_reference_index(reference_index)
      age:
        type: age_by_reference_index(reference_index)
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
