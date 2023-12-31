const fs   = require('fs');
const vm   = require('vm');
const yaml = require('js-yaml');
const ejs  = require('ejs');
const path = require('path');
const textEncoder = new TextEncoder();

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const FLT_MIN = 1.17549435082229e-038;
const M_PI    = 3.14159265358979323846264338327950288;

const setCharAt = (str,index,chr) => {
  if(index > str.length-1) return str;
  return str.substring(0,index) + chr + str.substring(index+1);
}

const escapeRegExpString = (str) => {
  const escaped_characters = /[|\\{}()[\]^$+*?.]/g;
  return str.replace(escaped_characters, '\\$&');
}

const stringReplaceAll = ( string, search_text, replace_text ) => {
  const search_for = new RegExp( escapeRegExpString(search_text), 'g' );
  return string.replace( search_for, replace_text );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 5) {
  console.error( process.argv[1] + ' [config.yml] [bsu_schema.yml] [outputfile]' );
  process.exit(1);
}

const config_filename_in           = process.argv[2];
const schema_filename_in           = process.argv[3];
const bsu_filename_out             = process.argv[4];
const config                       = yaml.load( fs.readFileSync( config_filename_in, 'utf8' ) );
const schema                       = yaml.load( fs.readFileSync( schema_filename_in, 'utf8' ) );

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const WriteU32 = ( u32, file ) => {
  const write_u32_buffer         = new ArrayBuffer(4);
  const write_u32                = new Uint32Array(write_u32_buffer);

  write_u32[0] = (u32 >>> 0);
  const write_len = fs.writeSync( file, write_u32, 0, write_u32.byteLength );
  return write_u32.byteLength;
}

const WriteU16 = ( u16, file ) => {
  const write_u16_buffer         = new ArrayBuffer(2);
  const write_u16                = new Uint16Array(write_u16_buffer);

  write_u16[0] = (u16 >>> 0);
  const write_len = fs.writeSync( file, write_u16, 0, write_u16.byteLength );
  return write_u16.byteLength;
}

const WriteF32 = ( f32, file ) => {
  const write_f32_buffer         = new ArrayBuffer(4);
  const write_f32                = new Float32Array(write_f32_buffer);

  write_f32[0] = f32;
  fs.writeSync( file, write_f32, 0, write_f32.byteLength );
  return write_f32.byteLength;
}

const WriteU8 = ( u8, file ) => {
  const write_u8_buffer          = new ArrayBuffer(1);
  const write_u8                 = new Uint8Array(write_u8_buffer);

  write_u8[0] = (u8 >>> 0);
  const write_len = fs.writeSync( file, write_u8, 0, write_u8.byteLength );
  return write_u8.byteLength;
}

const WriteUTF8 = ( string, file ) => {
  const write_string = new TextEncoder().encode(string);
  const write_string_len = fs.writeSync( file, write_string );
  const pad_len = (((((write_string_len+1) + 3)/4)|0)*4) - write_string_len;
  for (let i=0;i<pad_len;i++) {
    WriteU8( 0, file );
  }
  return write_string_len+pad_len;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const EvaluateExpression = ( expression_text, schema ) => {
  const context = { };
  schema.constants.forEach( constant => {
    context[ constant.id ] = constant.value;
  });
  vm.createContext( context );
  vm.runInContext( "var _result = " + expression_text, context );
  return context._result;
}

const WriteMagic = ( schema, file ) => {
  const magic_str           = schema.magic.value;
  const magic_size          = schema.magic.size;
  const write_string_buffer = new ArrayBuffer(magic_size);
  const write_string_u8     = new Uint8Array(write_string_buffer);

  write_string_u8.fill(0);
  textEncoder.encodeInto( magic_str, write_string_u8 );
  const write_len = fs.writeSync( file, write_string_u8, 0, magic_size );
  return magic_size;
}

const WriteSections = ( value, schema, data_section, mutable_section, file ) => {
  let size = 0;

  schema.sections.forEach( section => {
    const section_gid    = section.id;
    let section_offset = 0;
    if (section.mutable) { 
      section_offset = mutable_section.offsets[section_gid] || 0;
    } else {
      section_offset = data_section.offsets[section_gid] || 0;
    }
    size += WriteU32( section_offset, file );
  });

  schema.sections.forEach( section => {
    const section_gid = section.id;
    const storage_section = (section.mutable) ? mutable_section : data_section;
    storage_section.block.push( {
      gid: section_gid,
      write: () => {
        let size = 0;
        const section_value  = value ? value( section_gid ) : null;
        size += WriteBlock( section_gid, section_value, section, schema, storage_section, section.zi?true:false, file );
        return size;
      },
    });
  });

  return size;
}

const WriteBlock = ( gid, value, block, schema, storage_section, zi, file ) => {
  const container = block.container;
  const type_id   = block.type;
  let   size      = 0;

  if ( container == "static_array" ) {

    // is count statically part of schema?
    let count;
    if (block.hasOwnProperty('count')) {
      count = EvaluateExpression( block.count, schema ) || 0;
    } else {
      count = value("count");
    }

    const array_value  = value ? value("array") : null;
    const array_gid    = gid + '.static_array';
    const array_offset = ( count > 0 ) ? ( storage_section.offsets[array_gid] || 0 ) : 0;

    size += WriteU32( array_offset, file );
    size += WriteU32( count, file );

    storage_section.block.push( {
      gid: array_gid,
      write: () => {
        let size = 0;
        for (let i=0;i<count;i++) {
          const element_value = array_value ? array_value(i) : null;
          const element_gid   = array_gid + "[" + i + "]";
          const element_block = { type: type_id };
          size += WriteBlock( element_gid, element_value, element_block, schema, storage_section, zi, file );
        }
        return size;
      },
    });
  } else if ( type_id == "u32" ) {
    size += WriteU32( zi ? 0 : value, file );
  } else if ( type_id == "u8" ) {
    size += WriteU8( zi ? 0 : value, file );
  } else if ( type_id == "u16" ) {
    size += WriteU16( zi ? 0 : value, file );
  } else if ( type_id == "f32" ) {
    size += WriteF32( zi ? 0.0 : value, file );
  } else if ( type_id == "utf8" ) {

    const utf8_gid    = gid + '.utf8';
    const utf8_offset = storage_section.offsets[utf8_gid] || 0;

    size += WriteU32( utf8_offset, file );

    storage_section.block.push( {
      gid: utf8_gid,
      write: () => {
        let size = 0; 
        size += WriteUTF8( value, file );
        return size;
      },
    });

  }
  else {
    const type        = schema.types[type_id];
    const field_count = type.length;

    for (let i=0;i<field_count;i++) {

      const field_block = type[i];
      const field_id    = field_block.id;
      const field_gid   = gid + "." + field_id;
      let   field_value = null;
      if ( field_block.hasOwnProperty('value') ) {
        // value in schema
        field_value = EvaluateExpression( field_block.value, schema );
      } else {
        if ((!zi) && (!value)) {
          throw new Error("Error: Missing mapped value for '" + field_gid + "'");
        }
        // value in map
        field_value = value && value( field_id );
      }

      size += WriteBlock( field_gid, field_value, field_block, schema, storage_section, zi, file );
    }
  }
  return size;
}

const WriteBSUFile = ( get_value, schema, data_section, mutable_section, bsu_filename_out ) => {

  const file = fs.openSync( bsu_filename_out, "w" );
  let   size = 0;

  size += WriteMagic( schema, file );
  size += WriteU32( mutable_section.start, file );
  size += WriteU32( mutable_section.size, file );

  size += WriteSections( get_value, schema, data_section, mutable_section, file );

  data_section.start = size;
  if (data_section.block.length > 0) {
    let next_block = data_section.block.shift(); 
    while ( next_block ) {
      const next_gid   = next_block.gid; 
      const next_write = next_block.write;
      data_section.offsets[next_gid] = size;
      const next_block_size = next_write();
      const next_block_offset = size;
      size += next_block_size;
      next_block = data_section.block.shift(); 
    }
  }
  data_section.size = size - data_section.start;

  mutable_section.start = size;
  if (mutable_section.block.length > 0) {
    let next_block = mutable_section.block.shift(); 
    while ( next_block ) {
      const next_gid   = next_block.gid; 
      const next_write = next_block.write;
      mutable_section.offsets[next_gid] = size;
      const next_block_size = next_write();
      const next_block_offset = size;
      size += next_block_size;
      next_block = mutable_section.block.shift(); 
    }
  }
  mutable_section.size = size - mutable_section.start;

  fs.closeSync(file);
}

const WriteBSU = ( get_value, schema, bsu_filename_out ) => {
  const data_section = {
    start_offset: 0,
    size: 0,
    offsets: {},
    block: [],
  };
  const mutable_section = {
    start_offset: 0,
    size: 0,
    offsets: {},
    block: [],
  };
  WriteBSUFile( get_value, schema, data_section, mutable_section, bsu_filename_out );
  WriteBSUFile( get_value, schema, data_section, mutable_section, bsu_filename_out );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const map_vec2 = ( config_vec2 ) => {
  if ( config_vec2 ) {
    return ( id ) => {
      if ( id == "x" ) {
        return config_vec2[0];
      } else if ( id == "y" ) {
        return config_vec2[1];
      }
    }
  } else {
    return () => {
      return 0;
    }
  }
}

const map_play_area = ( config ) => {
  return ( id ) => {
    if ( id == "size" ) {
      return map_vec2( config.PlayArea.Size );
    }
  };
};

const map_asset_base_speed_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    const has_base_speed = config.BaseSpeed.hasOwnProperty(reference_name);
    const base_speed_f32 = has_base_speed ? config.BaseSpeed[reference_name] : 0.0;
    return base_speed_f32;
  };
}

const map_asset_base_speed_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_asset_base_speed_static_array_element( config );
    }
  };
}

const map_asset_base_health_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    const has_base_health = config.BaseHealth.hasOwnProperty(reference_name);
    const base_health_f32 = has_base_health ? config.BaseHealth[reference_name] : 0.0;
    return base_health_f32;
  };
}

const map_asset_base_health_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_asset_base_health_static_array_element( config );
    }
  };
}

const map_asset_base_size_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return map_vec2( config.BaseSize[reference_name] );
  };
}

const map_asset_base_size_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_asset_base_size_static_array_element( config );
    }
  };
}

const map_asset_names_static_array_element = ( config ) => {
  return (index) => {
    return config.References[index];
  };
}

const map_asset_names_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_asset_names_static_array_element( config );
    }
  };
}

const map_instance_count_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return config.InstanceCount[reference_name] || 0;
  };
}

const map_instance_count_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_instance_count_static_array_element( config );
    }
  };
}

const map_max_instance_count_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return config.MaxInstanceCount[reference_name] || 0;
  };
}

const map_max_instance_count_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_max_instance_count_static_array_element( config );
    }
  };
}

const map_instance_age_static_array_element_struct_array_f32_value_static_array_element = ( reference_name, config ) => {
  return (index) => {
    const f32_offset = config.Age[reference_name].F32Offset;
    const f32        = config.DataF32[f32_offset + index];
    return f32;
  };
}

const map_instance_age_static_array_element_struct_array_f32_value_static_array = ( reference_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.MaxInstanceCount[reference_name] || 0;
    }
    else if ( id == "array" ) {
      return map_instance_age_static_array_element_struct_array_f32_value_static_array_element( reference_name, config );
    }
  };
}

const map_instance_age_static_array_element_struct_array_f32 = ( reference_name, config ) => {
  return (field_name) => {
    if ( field_name == "value" ) {
      return map_instance_age_static_array_element_struct_array_f32_value_static_array( reference_name, config );
    }
  };
}

const map_instance_age_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return map_instance_age_static_array_element_struct_array_f32( reference_name, config );
  };
}

const map_instance_age_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_instance_age_static_array_element( config );
    }
  };
}

const map_instance_health_static_array_element_struct_array_f32_value_static_array_element = ( reference_name, config ) => {
  return (index) => {
    return 0;
  };
}

const map_instance_health_static_array_element_struct_array_f32_value_static_array = ( reference_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.MaxInstanceCount[reference_name] || 0;
    }
    else if ( id == "array" ) {
      return map_instance_health_static_array_element_struct_array_f32_value_static_array_element( reference_name, config );
    }
  };
}

const map_instance_health_static_array_element_struct_array_f32 = ( reference_name, config ) => {
  return (field_name) => {
    if ( field_name == "value" ) {
      return map_instance_health_static_array_element_struct_array_f32_value_static_array( reference_name, config );
    }
  };
}

const map_instance_health_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return map_instance_health_static_array_element_struct_array_f32( reference_name, config );
  };
}

const map_instance_health_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_instance_health_static_array_element( config );
    }
  };
}

const map_instance_location_static_array_element_struct_array_vec2_value_static_array_element = ( reference_name, config ) => {
  return (index) => {
    const vec2_offset = config.Location[reference_name].Vec2Offset;
    const vec2        = config.DataVec2[vec2_offset + index];
    return map_vec2( vec2 );  
  };
}

const map_instance_location_static_array_element_struct_array_vec2_value_static_array = ( reference_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.MaxInstanceCount[reference_name] || 0;
    }
    else if ( id == "array" ) {
      return map_instance_location_static_array_element_struct_array_vec2_value_static_array_element( reference_name, config );
    }
  };
}

const map_instance_location_static_array_element_struct_array_vec2 = ( reference_name, config ) => {
  return (field_name) => {
    if ( field_name == "value" ) {
      return map_instance_location_static_array_element_struct_array_vec2_value_static_array( reference_name, config );
    }
  };
}

const map_instance_location_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return map_instance_location_static_array_element_struct_array_vec2( reference_name, config );
  };
}

const map_instance_location_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_instance_location_static_array_element( config );
    }
  };
}

const map_instance_velocity_static_array_element_struct_array_vec2_value_static_array_element = ( reference_name, config ) => {
  return (index) => {
    return map_vec2( [ 0, 0 ] );  
  };
}

const map_instance_velocity_static_array_element_struct_array_vec2_value_static_array = ( reference_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.MaxInstanceCount[reference_name] || 0;
    }
    else if ( id == "array" ) {
      return map_instance_velocity_static_array_element_struct_array_vec2_value_static_array_element( reference_name, config );
    }
  };
}

const map_instance_velocity_static_array_element_struct_array_vec2 = ( reference_name, config ) => {
  return (field_name) => {
    if ( field_name == "value" ) {
      return map_instance_velocity_static_array_element_struct_array_vec2_value_static_array( reference_name, config );
    }
  };
}

const map_instance_velocity_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return map_instance_velocity_static_array_element_struct_array_vec2( reference_name, config );
  };
}

const map_instance_velocity_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_instance_velocity_static_array_element( config );
    }
  };
}

const map_asset_spawn_static_array_element_struct_spawn_at_each_static_array_element_struct_at_each = ( at_each, config ) => {
  return (id) => {
    if (id == "target_index") {
      const target_index = config.References.indexOf( at_each.AtEach );
      const direction    = EvaluateExpression( at_each.InitialDirection, schema ) || 0;
      const on_flag      = EvaluateExpression( at_each.OnFlag, schema ) || 0;
      const result       = direction | ( on_flag << 2) | ( target_index << 4 );
      return result; 
    } else if (id == "time_step") {
      return at_each.TimeStep;
    } else if (id == "time_next") {
      return 0;
    } else if (id == "location_offset") {
      return map_vec2( at_each.Offset );
    } else if (id == "pattern_width") {
      return at_each.PatternWidth;
    } else if (id == "pattern_u32_index") {
      return (at_each.PatternWidth > 0) ? at_each.PatternDataU32Offset : 0;
    }
   };
}

const map_asset_spawn_static_array_element_struct_spawn_at_each_static_array_element = ( reference_name, config ) => {
  return (index) => {
    const at_each_index = config.Spawn[reference_name].AtEachOffset;
    const at_each       = config.DataAtEach[ at_each_index + index ];

    return map_asset_spawn_static_array_element_struct_spawn_at_each_static_array_element_struct_at_each( at_each, config );
  };
}

const map_asset_spawn_static_array_element_struct_spawn_at_each_static_array = ( reference_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.Spawn.hasOwnProperty(reference_name) ? config.Spawn[reference_name].AtEachCount : 0;
    }
    else if ( id == "array" ) {
      return map_asset_spawn_static_array_element_struct_spawn_at_each_static_array_element( reference_name, config );
    }
  };
}

const map_asset_spawn_static_array_element_struct_spawn_at_group_static_array_element_struct_at_group = ( at_group, config ) => {
  return (id) => {
    if (id == "target_index") {
      const target_index = config.References.indexOf( at_group.AtGroup );
      const direction    = EvaluateExpression( at_group.InitialDirection, schema ) || 0;
      const on_flag      = EvaluateExpression( at_group.OnFlag, schema ) || 0;
      const result       = direction | ( on_flag << 2) | ( target_index << 4 );
      return result; 
    } else if (id == "time_step") {
      return at_group.TimeStep;
    } else if (id == "time_stop") {
      return at_group.TimeStop;
    } else if (id == "time_start") {
      return at_group.TimeStart;
    } else if (id == "time_next") {
      return 0;
    } else if (id == "location_offset") {
      return map_vec2( at_group.Offset );
    } else if (id == "pattern_count") {
      return at_group.PatternLength;
    } else if (id == "pattern_u8_index") {
      return (at_group.PatternLength > 0) ? at_group.PatternDataU8Offset : 0;
    }
   };
}

const map_asset_spawn_static_array_element_struct_spawn_at_group_static_array_element = ( reference_name, config ) => {
  return (index) => {
    const at_group_index = config.Spawn[reference_name].AtGroupOffset;
    const at_group       = config.DataAtGroup[ at_group_index + index ];

    return map_asset_spawn_static_array_element_struct_spawn_at_group_static_array_element_struct_at_group( at_group, config );
  };
}

const map_asset_spawn_static_array_element_struct_spawn_at_group_static_array = ( reference_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.Spawn.hasOwnProperty(reference_name) ? config.Spawn[reference_name].AtGroupCount : 0;
    }
    else if ( id == "array" ) {
      return map_asset_spawn_static_array_element_struct_spawn_at_group_static_array_element( reference_name, config );
    }
  };
}

const map_asset_spawn_static_array_element_struct_spawn = ( reference_name, config ) => {
  return (field_name) => {
    if ( field_name == "at_each" ) {
      return map_asset_spawn_static_array_element_struct_spawn_at_each_static_array( reference_name, config );
    }
    else if ( field_name == "at_group" ) {
      return map_asset_spawn_static_array_element_struct_spawn_at_group_static_array( reference_name, config );
    }
  };
}


const map_asset_spawn_static_array_element = ( config ) => {
  return (index) => {
    const reference_name = config.References[index];
    return map_asset_spawn_static_array_element_struct_spawn( reference_name, config );
  };
}

const map_asset_spawn_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.References.length;
    }
    else if ( id == "array" ) {
      return map_asset_spawn_static_array_element( config );
    }
  };
}

const map_pattern_u32_static_array_element = ( config ) => {
  return (index) => {
    return config.DataU32[index];
  };
}

const map_pattern_u32_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.DataU32.length;
    }
    else if ( id == "array" ) {
      return map_pattern_u32_static_array_element( config );
    }
  };
}

const map_pattern_u8_static_array_element = ( config ) => {
  return (index) => {
    return config.DataU8[index];
  };
}

const map_pattern_u8_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.DataU8.length;
    }
    else if ( id == "array" ) {
      return map_pattern_u8_static_array_element( config );
    }
  };
}

const map_collision_mod_health_static_array_element_struct_collision_mod_health_targets_static_array_element = ( source_asset_name, config ) => {
  return (index) => {
    return (id) => {
      if ( id == "target_asset_index" ) {
        const target_asset_name  = config.CollisionModHealth[source_asset_name][index].Target;
        const target_asset_index = config.References.indexOf( target_asset_name );
        return target_asset_index;
      }
      else if ( id == "amount" ) {
        const amount             = config.CollisionModHealth[source_asset_name][index].Amount;
        return amount;
      }
    };
  };
}

const map_collision_mod_health_static_array_element_struct_collision_mod_health_targets = ( source_asset_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.CollisionModHealth[source_asset_name].length;
    }
    else if ( id == "array" ) {
      return map_collision_mod_health_static_array_element_struct_collision_mod_health_targets_static_array_element( source_asset_name, config );
    }
  };
}

const map_collision_mod_health_static_array_element_struct_collision_mod_health = ( index, config ) => {
  return ( id ) => {
    if ( id == "source_asset_index" ) {
      const source_asset_name  = Object.keys(config.CollisionModHealth)[ index ];
      const source_asset_index = config.References.indexOf( source_asset_name );
      return source_asset_index;
    }
    else if ( id == "targets" ) {
      const source_asset_name  = Object.keys(config.CollisionModHealth)[ index ];
      return map_collision_mod_health_static_array_element_struct_collision_mod_health_targets( source_asset_name, config );
    }
  };
}

const map_collision_mod_health_static_array_element = ( config ) => {
  return (index) => {
    return map_collision_mod_health_static_array_element_struct_collision_mod_health( index, config );
  };
}

const map_collision_mod_health_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.CollisionModHealth ? Object.keys(config.CollisionModHealth).length : 0;
    }
    else if ( id == "array" ) {
      return map_collision_mod_health_static_array_element( config );
    }
  };
}

const map_collision_mod_flags_static_array_element_struct_collision_mod_flags_targets_static_array_element = ( source_asset_name, config ) => {
  return (index) => {
    return (id) => {
      if ( id == "target_asset_index" ) {
        const target_asset_name  = config.CollisionModFlag[source_asset_name][index].Target;
        const target_asset_index = config.References.indexOf( target_asset_name );
        return target_asset_index;
      }
      else if ( id == "value" ) {
        const flag_text = config.CollisionModFlag[source_asset_name][index].Value;
        const flag_value = EvaluateExpression( flag_text, schema ) || 0;
        return flag_value;
      }
    };
  };
}

const map_collision_mod_flags_static_array_element_struct_collision_mod_flags_targets = ( source_asset_name, config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.CollisionModFlag[source_asset_name].length;
    }
    else if ( id == "array" ) {
      return map_collision_mod_flags_static_array_element_struct_collision_mod_flags_targets_static_array_element( source_asset_name, config );
    }
  };
}

const map_collision_mod_flags_static_array_element_struct_collision_mod_flags = ( index, config ) => {
  return ( id ) => {
    if ( id == "source_asset_index" ) {
      const source_asset_name  = Object.keys(config.CollisionModFlag)[ index ];
      const source_asset_index = config.References.indexOf( source_asset_name );
      return source_asset_index;
    }
    else if ( id == "targets" ) {
      const source_asset_name  = Object.keys(config.CollisionModFlag)[ index ];
      return map_collision_mod_flags_static_array_element_struct_collision_mod_flags_targets( source_asset_name, config );
    }
  };
}

const map_collision_mod_flags_static_array_element = ( config ) => {
  return (index) => {
    return map_collision_mod_flags_static_array_element_struct_collision_mod_flags( index, config );
  };
}

const map_collision_mod_flags_static_array = ( config ) => {
  return ( id ) => {
    if ( id == "count" ) {
      return config.CollisionModFlag ? Object.keys(config.CollisionModFlag).length : 0;
    }
    else if ( id == "array" ) {
      return map_collision_mod_flags_static_array_element( config );
    }
  };
}

const map_bsu = ( config ) => {
  const get_value = id => {
    if ( id == "asset_names" ) {
      return map_asset_names_static_array( config );
    } else if ( id == "asset_base_size" ) {
      return map_asset_base_size_static_array( config );
    } else if ( id == "asset_base_speed" ) {
      return map_asset_base_speed_static_array( config );
    } else if ( id == "asset_base_health" ) {
      return map_asset_base_health_static_array( config );
    } else if ( id == "instance_count" ) {
      return map_instance_count_static_array( config );
    } else if ( id == "max_instance_count" ) {
      return map_max_instance_count_static_array( config );
    } else if ( id == "play_area" ) {
      return map_play_area( config );
    } else if ( id == "instance_age" ) {
      return map_instance_age_static_array( config );
    } else if ( id == "instance_health" ) {
      return map_instance_health_static_array( config );
    } else if ( id == "instance_location" ) {
      return map_instance_location_static_array( config );
    } else if ( id == "instance_velocity" ) {
      return map_instance_velocity_static_array( config );
    } else if ( id == "asset_spawn" ) {
      return map_asset_spawn_static_array( config );
    } else if ( id == "pattern_u32" ) {
      return map_pattern_u32_static_array( config );
    } else if ( id == "pattern_u8" ) {
      return map_pattern_u8_static_array( config );
    } else if ( id == "collision_mod_health" ) {
      return map_collision_mod_health_static_array( config );
    } else if ( id == "collision_mod_flags" ) {
      return map_collision_mod_flags_static_array( config );
    } else {
      return null;
    }
  }
  return get_value;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

WriteBSU( map_bsu(config), schema, bsu_filename_out );
