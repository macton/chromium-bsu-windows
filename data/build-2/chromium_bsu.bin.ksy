
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
