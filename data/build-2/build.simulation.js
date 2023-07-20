const fs   = require('fs');
const vm   = require('vm');
const yaml = require('js-yaml');
const ejs  = require('ejs');
const path = require('path');
const textEncoder = new TextEncoder();

const write_path_string_buffer = new ArrayBuffer(256);
const write_path_string_u8     = new Uint8Array(write_path_string_buffer);
const write_name_string_buffer = new ArrayBuffer(64);
const write_name_string_u8     = new Uint8Array(write_name_string_buffer);
const write_u32_buffer         = new ArrayBuffer(4);
const write_u32                = new Uint32Array(write_u32_buffer);
const write_u8_buffer          = new ArrayBuffer(1);
const write_u8                 = new Uint8Array(write_u8_buffer);
const write_f32_buffer         = new ArrayBuffer(4);
const write_f32                = new Float32Array(write_f32_buffer);
const write_vec2_buffer        = new ArrayBuffer(8);
const write_vec2               = new Float32Array(write_vec2_buffer);

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

const PreProcess_Spawn_AtEach_Pattern = ( pattern_in ) => {
  const word_count = ((pattern_in.length+31)/32)|0;
  const words      = Array(word_count).fill(0);
  for (let i=0;i<pattern_in.length;i++) {
    if ( pattern_in[i] == '*' ) {
      const word_ndx = (i/32)|0;
      const bit_ndx  = i-(word_ndx*32);
      words[word_ndx] = words[word_ndx] | (1 << i);
    }
  }
  return {
    PatternWidth: pattern_in.length,
    Pattern: words.map( word => '0x' + (word >>> 0).toString(16).padStart(8,'0') ),
  }
}

const PreProcess_Spawn_AtEach = ( spawn_in, config_in ) => {
  return {
    AtEach:   spawn_in.AtEach,
    TimeStep: spawn_in.TimeStep,
    Offset:   spawn_in.Offset,
    ...PreProcess_Spawn_AtEach_Pattern( spawn_in.Pattern ),
  };
}
 
const GetMaxInstanceCount = (group_name, config_in) => {
  if ( config_in.hasOwnProperty('MaxInstanceCount') && config_in.MaxInstanceCount.hasOwnProperty(group_name) ) {
    return config_in.MaxInstanceCount[group_name];
  }
  if ( config_in.hasOwnProperty('Location') && config_in.Location.hasOwnProperty(group_name) ) {
    return config_in.Location[group_name].length;
  }
  return 0;
}

const PreProcess_Spawn_AtGroup_Expression = ( spawn_in, config_in ) => {
  const location_count  = GetMaxInstanceCount( spawn_in.AtGroup, config_in );
  const code            = 'result = ' + config_in.Expression[ spawn_in.Expression ];
  const context = {
    M_PI:    M_PI,
    FLT_MIN: FLT_MIN,
    sin:     Math.sin,
    cos:     Math.cos,
    ...spawn_in,
  };
  vm.createContext(context);

  if ( !spawn_in.hasOwnProperty("Offset") ) {
    spawn_in.Offset = [ 0, 0 ];
  }

  const pattern        = [];
  const time_start     = spawn_in.TimeStart;
  const time_stop      = spawn_in.TimeStop;
  const time_step      = spawn_in.TimeStep;
  for (time=time_start;time<=time_stop;time+=time_step) {
    context.Time = time;
    vm.runInContext( code, context );

    const index = Math.min( ((0.5 * (context.result + 1.0)) * location_count)|0, location_count-1 );

    pattern.push( '0x' + (index >>> 0).toString(16).padStart(2,'0') );
  }
  return { 
    AtGroup:   spawn_in.AtGroup,
    TimeStep:  spawn_in.TimeStep,
    TimeStart: spawn_in.TimeStart,
    TimeStop:  spawn_in.TimeStop,
    Offset:    [...spawn_in.Offset],
    Pattern:   [...pattern],
  };
}

const PreProcess_Spawn_AtGroup_Formation = ( spawn_in, config_in ) => {
  let formation = config_in.Formation[ spawn_in.Formation ].map( row => [...row] );
  let slices = [];

  if ( !spawn_in.hasOwnProperty("TimeStop") ) {
    const time_step  = spawn_in.TimeStep;
    const time_start = spawn_in.TimeStart;
    const time_stop  = time_start + (time_step * formation.length);
    spawn_in.TimeStop = time_stop;
  }
  if ( !spawn_in.hasOwnProperty("Offset") ) {
    spawn_in.Offset = [ 0, 0 ];
  }

  let found_formation_slice = false;
  do {
    let slice_pattern = [];
    found_formation_slice = false;

    for (y=0;y<formation.length;y++) {
      let found_slice_row = false;

      // find the index of the group to spawn at for this time slice
      for (x=0;(x<formation[y].length)&&(!found_slice_row);x++) {
        if ( formation[y][x] == '*' ) {
          const index = x;
          found_slice_row = true;
          slice_pattern.push( '0x' + (index >>> 0).toString(16).padStart(2,'0') );
          formation[y][x] = '.';
        }
      }

      if (!found_slice_row) {
        slice_pattern.push( '0xff' );
      } else {
        found_formation_slice = true;
      }
    }  

    if (found_formation_slice) {
      slices.push( { 
        AtGroup:   spawn_in.AtGroup,
        TimeStep:  spawn_in.TimeStep,
        TimeStart: spawn_in.TimeStart,
        TimeStop:  spawn_in.TimeStop,
        Offset:    [...spawn_in.Offset],
        Pattern:   [...slice_pattern],
      });
    }

  } while ( found_formation_slice );
  return slices;
}

const PreProcess_Spawn = ( spawn_in, config_in ) => {
  const spawn_out = {};
  Object.keys(spawn_in).forEach( spawn_asset_name => {
    spawn_out[spawn_asset_name] = spawn_in[ spawn_asset_name ].flatMap( spawn => {
      if ( spawn.hasOwnProperty('AtEach') ) {
        return PreProcess_Spawn_AtEach( spawn, config_in );
      }
      if ( spawn.hasOwnProperty('AtGroup') && spawn.hasOwnProperty('Expression') ) {
        return PreProcess_Spawn_AtGroup_Expression( spawn, config_in );
      }
      if ( spawn.hasOwnProperty('AtGroup') && spawn.hasOwnProperty('Formation') ) {
        return PreProcess_Spawn_AtGroup_Formation( spawn, config_in );
      }
      return [];
    });
  });
  return { Spawn: spawn_out };
}

const PreProcess_GatherReferences = ( config_in ) => {
  const references = new Set();
  config_in.MaxInstanceCount && Object.keys(config_in.MaxInstanceCount).forEach( references.add, references );
  config_in.Location         && Object.keys(config_in.Location).forEach( references.add, references )
  config_in.BaseSize         && Object.keys(config_in.BaseSize).forEach( references.add, references );
  config_in.BaseSpeed        && Object.keys(config_in.BaseSpeed).forEach( references.add, references );
  config_in.Spawn            && Object.keys(config_in.Spawn).forEach( references.add, references );
  return [...references];
}

const PreProcess = ( config_in ) => {

  //
  // Pass-1
  //

  const preprocessor = {
    PlayArea:         value => { return { PlayArea: value } },
    MaxInstanceCount: value => { return { MaxInstanceCount: value } },
    Location:         value => { return { Location: value } },  
    BaseSize:         value => { return { BaseSize: value } }, 
    BaseSpeed:        value => { return { BaseSpeed: value } }, 
    Spawn:            PreProcess_Spawn,
  };
  config_out = {
    References: PreProcess_GatherReferences( config_in ),
  };
  Object.keys(config_in).forEach( key => {
    if ( preprocessor.hasOwnProperty(key) ) {
      const value = config_in[key];
      config_out = { ...config_out, ...preprocessor[key](value, config_in) };
    }
  });

  //
  // Pass-2 
  //   - Move index patterns to shaded table
  //

  let data_u8      = [];
  let data_u32     = [];
  let data_atgroup = [];
  let data_ateach  = [];
  let data_vec2    = [];
  let data_f32 = [];
 
  if ( config_out.hasOwnProperty('Spawn') ) {
    Object.keys(config_out.Spawn).forEach( spawn_asset_name => {
      config_out.Spawn[spawn_asset_name].forEach( spawn => {
        if ( spawn.hasOwnProperty('AtGroup') && spawn.hasOwnProperty('Pattern') ) {
          const pattern = [ ...spawn.Pattern ];
          delete spawn.Pattern;
  
                    
          const data_u8_str = data_u8.join('');
          const pattern_str  = pattern.join('');
          let pattern_index = data_u8_str.indexOf( pattern_str );
    
          if ( pattern_index == -1 ) {
            pattern_index = data_u8.length;
            data_u8       = [ ...data_u8, ...pattern ];
          } else {
            pattern_index = pattern_index / 4; // "0x--"
          }
    
          spawn.PatternLength = pattern.length;
          spawn.PatternDataU8Offset = pattern_index;
        }
        else if ( spawn.hasOwnProperty('AtEach') && spawn.hasOwnProperty('Pattern') ) {
          const pattern = [ ...spawn.Pattern ];
          delete spawn.Pattern;
    
          const data_u32_str = data_u32.join('');
          const pattern_str  = pattern.join('');
          let pattern_index = data_u32_str.indexOf( pattern_str );
     
          if ( pattern_index == -1 ) {
            pattern_index = data_u32.length;
            data_u32      = [ ...data_u32, ...pattern ];
          } else {
            pattern_index = pattern_index / 10; // "0x--------"
          }
    
          spawn.PatternDataU32Offset = pattern_index;
        }
      });
    });
  }

  if ( config_out.hasOwnProperty('Spawn') ) {
    Object.keys(config_out.Spawn).forEach( spawn_asset_name => {
      let ateach_offset  = data_ateach.length;
      let atgroup_offset = data_ateach.length;
      let ateach_count   = 0;
      let atgroup_count  = 0;

      config_out.Spawn[spawn_asset_name].forEach( spawn => {
        if ( spawn.hasOwnProperty('AtGroup') ) {
          data_atgroup.push( spawn );
          atgroup_count++;
        }
        if ( spawn.hasOwnProperty('AtEach') ) {
          data_ateach.push( spawn );
          ateach_count++;
        }
      });

      config_out.Spawn[spawn_asset_name] = {
        AtEachOffset:   ateach_offset,
        AtEachCount:    ateach_count,
        AtGroupOffset:  atgroup_offset,
        AtGroupCount:   atgroup_count,
      };
    });
  }

  if ( config_out.hasOwnProperty('Location') || config_out.hasOwnProperty('MaxInstanceCount') ) {
    if (!config_out.hasOwnProperty('InstanceCount')) {
      config_out.InstanceCount = {};
    }
    if (!config_out.hasOwnProperty('MaxInstanceCount')) {
      config_out.MaxInstanceCount = {};
    }

    config_out.References.forEach( asset_name => {
      let vec2_offset = data_vec2.length;
      let vec2_count  = GetMaxInstanceCount( asset_name, config_out );
      let used_count  = 0;
      
      if ( config_out.Location.hasOwnProperty( asset_name ) ) {
        config_out.Location[asset_name].forEach( location => {
          data_vec2.push( [...location] );
        });
        used_count = config_out.Location[asset_name].length;
      } else {
        for (let i=0;i<vec2_count;i++) {
          data_vec2.push( [0.0,0.0] );
        }
      }
      config_out.Location[ asset_name ] = { 
        Vec2Offset: vec2_offset,
      };

      config_out.MaxInstanceCount[ asset_name ] = vec2_count;
      config_out.InstanceCount[ asset_name ]    = used_count;
    }); 
  }

  // Age
  {
    config_out.Age = {};

    config_out.References.forEach( asset_name => {
      let f32_offset = data_f32.length;
      let f32_count  = GetMaxInstanceCount( asset_name, config_out );
      
      for (let i=0;i<f32_count;i++) {
        data_f32.push( 0.0 );
      }
      config_out.Age[ asset_name ] = { 
        F32Offset: f32_offset,
      };
    }); 
  }

  if ( data_u8.length > 0 )  {
    config_out.DataU8 = data_u8;
  }
  if ( data_u32.length > 0 ) {
    config_out.DataU32 = data_u32;
  }
  if (data_ateach.length > 0 ) {
    config_out.DataAtEach = data_ateach;
  }
  if (data_atgroup.length > 0 ) {
    config_out.DataAtGroup = data_atgroup;
  }
  if (data_vec2.length > 0 ) {
    config_out.DataVec2 = data_vec2;
  }
  if (data_f32.length > 0 ) {
    config_out.DataF32 = data_f32;
  }

  return config_out;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const WritePathStringUTF8 = ( string, file ) => {
  write_path_string_u8.fill(0);
  textEncoder.encodeInto( string, write_path_string_u8 );
  fs.writeSync( file, write_path_string_u8 );
  return write_path_string_buffer.byteLength;
}

const WriteNameStringUTF8 = ( string, file, position ) => {
  write_name_string_u8.fill(0);
  textEncoder.encodeInto( string, write_name_string_u8 );
  const write_len = fs.writeSync( file, write_name_string_u8, 0, write_name_string_u8.byteLength, position );
  return write_name_string_buffer.byteLength;
}

const WriteU32 = ( u32, file, position ) => {
  write_u32[0] = (u32 >>> 0);
  const write_len = fs.writeSync( file, write_u32, 0, write_u32.byteLength, position );
  return write_u32.byteLength;
}

const WriteU8 = ( u8, file, position ) => {
  write_u8[0] = (u8 >>> 0);
  const write_len = fs.writeSync( file, write_u8, 0, write_u8.byteLength, position );
  return write_u8.byteLength;
}

const WriteF32 = ( f32, file, position ) => {
  write_f32[0] = f32;
  fs.writeSync( file, write_f32, 0, write_f32.byteLength, position );
  return write_f32.byteLength;
}

const WriteVec2 = ( vec2, file, position ) => {
  write_vec2[0] = vec2[0];
  write_vec2[1] = vec2[1];
  fs.writeSync( file, write_vec2, 0, write_vec2.byteLength, position );
  return write_vec2.byteLength;
}

const WriteFill = ( size, file ) => {
  const aligned_size = (((size + 3)/4)|0)*4;
  const count        = aligned_size-size;

  write_u8[0] = 0;
  for (let i=0;i<count;i++) {
    fs.writeSync( file, write_u8 );
  }
  return count;
}

const WriteBinaryConfig_References = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.References.length, file );
  config.References.forEach( reference_name => {
    size += WriteNameStringUTF8( reference_name, file );
  });
  return size;
}

const WriteBinaryConfig_PlayArea = ( config, file ) => {
  let size = 0;
  size += WriteVec2( config.PlayArea.Size, file );
  return size;
}

const WriteBinaryConfig_Location = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    const location = config.Location[reference_name];
    size += WriteU32( location.Vec2Offset, file );
  });
  return size;
}

const WriteBinaryConfig_BaseSize = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    if (config.BaseSize.hasOwnProperty(reference_name)) {
      size += WriteVec2( config.BaseSize[reference_name], file );
    } else {
      size += WriteVec2( [0.0,0.0], file );
    }
  });
  return size;
}

const WriteBinaryConfig_BaseSpeed = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    if (config.BaseSpeed.hasOwnProperty(reference_name)) {
      size += WriteF32( config.BaseSpeed[reference_name], file );
    } else {
      size += WriteF32( 0.0, file );
    }
  });
  return size;
}

const WriteBinaryConfig_Spawn = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    if (!config.Spawn.hasOwnProperty(reference_name)) {
      size += WriteU32( 0, file );
      size += WriteU32( 0, file );
      size += WriteU32( 0, file );
      size += WriteU32( 0, file );
    } else {
      const spawn = config.Spawn[reference_name];
      size += WriteU32( spawn.AtEachOffset, file );
      size += WriteU32( spawn.AtEachCount, file );
      size += WriteU32( spawn.AtGroupOffset, file );
      size += WriteU32( spawn.AtGroupCount, file );
    }
  });
  return size;
}

const WriteBinaryConfig_Age = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    const f32_offset = config.Age[reference_name].F32Offset;
    size += WriteU32( f32_offset, file );
  });
  return size;
}

const WriteBinaryConfig_MaxInstanceCount = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    const max_instance_count = GetMaxInstanceCount( reference_name, config );
    size += WriteU32( max_instance_count, file );
  });
  return size;
}

const WriteBinaryConfig_InstanceCount = ( config, file ) => {
  let size = 0;
  config.References.forEach( reference_name => {
    const instance_count = config.InstanceCount[reference_name];
    size += WriteU32( instance_count, file );
  });
  return size;
}

const WriteBinaryConfig_DataU8 = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.DataU8.length, file );
  config.DataU8.forEach( u8 => {
    size += WriteU8( u8, file );
  });
  return size;
}

const WriteBinaryConfig_DataU32 = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.DataU32.length, file );
  config.DataU32.forEach( u32 => {
    size += WriteU32( u32, file );
  });
  return size;
}

const WriteBinaryConfig_DataVec2 = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.DataVec2.length, file );
  config.DataVec2.forEach( vec2 => {
    size += WriteVec2( vec2, file );
  });
  return size;
}

const WriteBinaryConfig_DataF32 = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.DataF32.length, file );
  config.DataF32.forEach( f32 => {
    size += WriteF32( f32, file );
  });
  return size;
}

const WriteBinaryConfig_DataAtEach = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.DataAtEach.length, file );
  config.DataAtEach.forEach( at_each => {
    size += WriteF32( 0, file ); // NextSpawnTime
    size += WriteF32( at_each.TimeStep, file );
    size += WriteVec2( at_each.Offset, file );
    size += WriteU32( at_each.PatternWidth, file );
    size += WriteU32( at_each.PatternDataU32Offset, file );
  });
  return size;
}

const WriteBinaryConfig_DataAtGroup = ( config, file ) => {
  let size = 0;
  size += WriteU32( config.DataAtGroup.length, file );
  config.DataAtGroup.forEach( at_group => {
    size += WriteF32( 0, file ); // NextSpawnTime
    size += WriteF32( at_group.TimeStep, file );
    size += WriteF32( at_group.TimeStart, file );
    size += WriteF32( at_group.TimeStop, file );
    size += WriteVec2( at_group.Offset, file );
    size += WriteU32( at_group.PatternLength, file );
    size += WriteU32( at_group.PatternDataU8Offset, file );
  });
  return size;
}

const WriteBinaryConfig_TOC_Fill = ( toc, config, file ) => {
  let size = 0;

  size += WriteNameStringUTF8( "CHROMIUM.BSU.2023", file );
  size += WriteU32( toc.References, file );
  size += WriteU32( toc.MaxInstranceCount, file );
  size += WriteU32( toc.InstranceCount, file );
  size += WriteU32( toc.PlayArea, file );
  size += WriteU32( toc.Location, file );
  size += WriteU32( toc.BaseSize, file );
  size += WriteU32( toc.BaseSpeed, file );
  size += WriteU32( toc.Spawn, file );
  size += WriteU32( toc.Age, file );
  size += WriteU32( toc.DataU8, file);
  size += WriteU32( toc.DataU32, file);
  size += WriteU32( toc.DataAtEach, file);
  size += WriteU32( toc.DataAtGroup, file);
  size += WriteU32( toc.DataVec2, file);
  size += WriteU32( toc.DataF32, file);

  return size;
}

const WriteBinaryConfig_TOC = ( toc, config, file ) => {
  let size = 0;

  size += WriteNameStringUTF8( "CHROMIUM.BSU.2023", file, size );
  size += WriteU32( toc.References, file, size );
  size += WriteU32( toc.MaxInstanceCount, file, size );
  size += WriteU32( toc.InstanceCount, file, size );
  size += WriteU32( toc.PlayArea, file, size );
  size += WriteU32( toc.Location, file, size );
  size += WriteU32( toc.BaseSize, file, size );
  size += WriteU32( toc.BaseSpeed, file, size );
  size += WriteU32( toc.Spawn, file, size );
  size += WriteU32( toc.Age, file, size );
  size += WriteU32( toc.DataU8, file, size );
  size += WriteU32( toc.DataU32, file, size );
  size += WriteU32( toc.DataAtEach, file, size );
  size += WriteU32( toc.DataAtGroup, file, size );
  size += WriteU32( toc.DataVec2, file, size );
  size += WriteU32( toc.DataF32, file, size );

  return size;
}

const WriteBinaryConfig = ( config, filename_out ) => {
  const file = fs.openSync( filename_out, "w" );
  const toc = {
    References: 0,
    MaxInstanceCount: 0,
    PlayArea: 0,
    Location: 0,
    BaseSize: 0,
    BaseSpeed: 0,
    Spawn: 0,
    Age: 0,
    DataU8: 0,
    DataU32: 0,
    DataAtEach: 0,
    DataAtGroup: 0,
    DataVec2: 0,
    DataF32: 0,
  };

  let size = 0;
  size += WriteBinaryConfig_TOC_Fill( toc, config, file );
  size += WriteFill( size, file );

  toc.References = size;
  size += WriteBinaryConfig_References( config, file );
  size += WriteFill( size, file );

  toc.MaxInstanceCount = size;
  size += WriteBinaryConfig_MaxInstanceCount( config, file );
  size += WriteFill( size, file );

  toc.InstanceCount = size;
  size += WriteBinaryConfig_InstanceCount( config, file );
  size += WriteFill( size, file );

  toc.PlayArea = size;
  size += WriteBinaryConfig_PlayArea( config, file );
  size += WriteFill( size, file );

  toc.Location = size;
  size += WriteBinaryConfig_Location( config, file );
  size += WriteFill( size, file );

  toc.BaseSize = size;
  size += WriteBinaryConfig_BaseSize( config, file );
  size += WriteFill( size, file );

  toc.BaseSpeed = size;
  size += WriteBinaryConfig_BaseSpeed( config, file );
  size += WriteFill( size, file );

  toc.Spawn = size;
  size += WriteBinaryConfig_Spawn( config, file );
  size += WriteFill( size, file );

  toc.Age = size;
  size += WriteBinaryConfig_Age( config, file );
  size += WriteFill( size, file );

  toc.DataU8 = size;
  size += WriteBinaryConfig_DataU8( config, file );
  size += WriteFill( size, file );

  toc.DataU32 = size;
  size += WriteBinaryConfig_DataU32( config, file );
  size += WriteFill( size, file );

  toc.DataAtEach = size;
  size += WriteBinaryConfig_DataAtEach( config, file );
  size += WriteFill( size, file );

  toc.DataAtGroup = size;
  size += WriteBinaryConfig_DataAtGroup( config, file );
  size += WriteFill( size, file );

  toc.DataVec2 = size;
  size += WriteBinaryConfig_DataVec2( config, file );
  size += WriteFill( size, file );

  toc.DataF32 = size;
  size += WriteBinaryConfig_DataF32( config, file );
  size += WriteFill( size, file );

  WriteBinaryConfig_TOC( toc, config, file );
  fs.closeSync(file);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 3) {
  console.error( process.argv[1] + ' [input file]' );
  process.exit(1);
}

const filename_in             = process.argv[2];
const config                  = PreProcess( yaml.load( fs.readFileSync( filename_in, 'utf8' ) ) );
const filename_in_extname     = path.extname( filename_in );
const filename_in_basename    = path.basename( filename_in, filename_in_extname );
const filename_in_dirname     = path.dirname( filename_in );
const preprocess_filename_out = path.resolve( filename_in_dirname, filename_in_basename + ".preprocessed" + filename_in_extname );
const bin_filename_out        = path.resolve( filename_in_dirname, filename_in_basename + ".bsu" );

fs.writeFileSync( preprocess_filename_out, yaml.dump(config) );
WriteBinaryConfig( config, bin_filename_out );
