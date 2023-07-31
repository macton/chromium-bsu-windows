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
    AtEach:             spawn_in.AtEach,
    TimeStep:           spawn_in.TimeStep,
    Offset:             spawn_in.Offset,
    InitialDirection:   spawn_in.InitialDirection,
    OnFlag:             spawn_in.OnFlag,
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
    AtGroup:           spawn_in.AtGroup,
    TimeStep:          spawn_in.TimeStep,
    TimeStart:         spawn_in.TimeStart,
    TimeStop:          spawn_in.TimeStop,
    InitialDirection:  spawn_in.InitialDirection,
    OnFlag:             spawn_in.OnFlag,
    Offset:            [...spawn_in.Offset],
    Pattern:           [...pattern],
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
        AtGroup:           spawn_in.AtGroup,
        TimeStep:          spawn_in.TimeStep,
        TimeStart:         spawn_in.TimeStart,
        TimeStop:          spawn_in.TimeStop,
        OnFlag:            spawn_in.OnFlag,
        InitialDirection:  spawn_in.InitialDirection,
        Offset:            [...spawn_in.Offset],
        Pattern:           [...slice_pattern],
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
    PlayArea:            value => { return { PlayArea: value } },
    MaxInstanceCount:    value => { return { MaxInstanceCount: value } },
    Location:            value => { return { Location: value } },  
    BaseSize:            value => { return { BaseSize: value } }, 
    BaseSpeed:           value => { return { BaseSpeed: value } }, 
    BaseHealth:          value => { return { BaseHealth: value } }, 
    CollisionModHealth:  value => { return { CollisionModHealth: value } }, 
    CollisionModFlag:    value => { return { CollisionModFlag: value } }, 
    Spawn:               PreProcess_Spawn,
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
      let atgroup_offset = data_atgroup.length;
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

if (process.argv.length < 4) {
  console.error( process.argv[1] + ' [input file] [output file]' );
  process.exit(1);
}

const filename_in   = process.argv[2];
const filename_out  = process.argv[3];
const config        = PreProcess( yaml.load( fs.readFileSync( filename_in, 'utf8' ) ) );

fs.writeFileSync( filename_out, yaml.dump(config) );
