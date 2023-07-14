meta:
  id: bsu_file
  endian: le

seq:
  - id: toc
    type: toc

types:
  name_string:
    seq:
      - type: str
        encoding: ASCII
        size: 64
  toc:
    seq:
      - id: magic
        type: name_string
      - id: reference_offset
        type: u4
      - id: playarea_offset
        type: u4
      - id: max_location_count_offset
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
      - id: data_ateach_offset
        type: u4
      - id: data_atgroup_offset
        type: u4
