meta:
  id: sim
  endian: le
seq:
  - id: play_area
    type: vec2
  - id: assets
    type: sim_asset_table
  - id: generators
    type: generator_table
types:
  vec2:
    seq:
      - id: x
        type: f4
      - id: y
        type: f4
  sim:
    seq:
      - id: play_area
        type: vec2
      - id: assets
        type: sim_asset_table
      - id: generators
        type: generator_table
  sim_asset_table:
    seq:
      - id: count
        type: u4
      - id: name_offset
        type: u4
      - id: size_offset
        type: u4
      - id: speed_offset
        type: u4
      - id: base_health_offset
        type: u4
      - id: max_instance_count_offset
        type: u4
    instances:
      name_value:
        type: u4
        pos: name_offset
        repeat: expr
        repeat-expr: count
      size_value:
        type: vec2
        pos: size_offset
        repeat: expr
        repeat-expr: count
      speed_value:
        type: f4
        pos: speed_offset
        repeat: expr
        repeat-expr: count
      base_health_value:
        type: u4
        pos: base_health_offset
        repeat: expr
        repeat-expr: count
      max_instance_count_value:
        type: u4
        pos: max_instance_count_offset
        repeat: expr
        repeat-expr: count
  generator_table:
    seq:
      - id: each_count
        type: u4
      - id: group_count
        type: u4
      - id: each_offset
        type: u4
      - id: group_offset
        type: u4
    instances:
      each_value:
        type: generator_each
        pos: each_offset
        repeat: expr
        repeat-expr: each_count
      group_value:
        type: generator_group
        pos: group_offset
        repeat: expr
        repeat-expr: group_count
  generator_each:
    seq:
      - id: asset_index
        type: u4
      - id: target_index
        type: u4
      - id: time_step
        type: f4
      - id: time_next
        type: f4
      - id: location_offset
        type: vec2
      - id: pattern_width
        type: u4
      - id: pattern_offset
        type: u4
    instances:
      pattern_value:
        type: u4
        pos: pattern_offset
        repeat: expr
        repeat-expr: ( pattern_width + 31 ) / 32
  generator_group:
    seq:
      - id: asset_index
        type: u4
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
        type: vec2
      - id: pattern_count
        type: u4
      - id: pattern_offset
        type: u4
    instances:
      pattern_value:
        type: u1
        pos: pattern_offset
        repeat: expr
        repeat-expr: pattern_count
  ui_sim:
    seq:
      - id: play_area
        type: vec2
      - id: assets_offset
        type: u4
  ui_sim_asset:
    seq:
      - id: name_offset
        type: u4
      - id: size
        type: vec2
      - id: speed
        type: f4
      - id: base_health
        type: u4
      - id: max_instance_count
        type: u4
    instances:
      name_value:
        type: str
        encoding: UTF-8
        pos: name_offset
        terminator: 0
