const fs   = require('fs');
const vm   = require('vm');
const yaml = require('js-yaml');
const ejs  = require('ejs');
const path = require('path');

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
    Asset:    spawn_in.Asset,
    AtEach:   spawn_in.AtEach,
    TimeStep: spawn_in.TimeStep,
    Offset:   spawn_in.Offset,
    ...PreProcess_Spawn_AtEach_Pattern( spawn_in.Pattern ),
  };
}
 
const GetGroupLocationCount = (group_name, config_in) => {
  if ( config_in.hasOwnProperty('MaxLocationCount') && config_in.MaxLocationCount.hasOwnProperty(group_name) ) {
    return config_in.MaxLocationCount[group_name];
  }
  if ( config_in.hasOwnProperty('Location') && config_in.Location.hasOwnProperty(group_name) ) {
    return config_in.Location[group_name].length;
  }
  return 0;
}

const PreProcess_Spawn_AtGroup_Expression = ( spawn_in, config_in ) => {
  const location_count  = GetGroupLocationCount( spawn_in.AtGroup, config_in );
  const code            = 'result = ' + config_in.Expression[ spawn_in.Expression ];
  const context = {
    M_PI:    M_PI,
    FLT_MIN: FLT_MIN,
    sin:     Math.sin,
    cos:     Math.cos,
    ...spawn_in,
  };
  vm.createContext(context);

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
    Asset:     spawn_in.Asset,
    AtGroup:   spawn_in.AtGroup,
    TimeStep:  spawn_in.TimeStep,
    TimeStart: spawn_in.TimeStart,
    TimeStop:  spawn_in.TimeStop,
    Pattern:   pattern,
  };
}

const PreProcess_Spawn_AtGroup = ( spawn_in, config_in ) => {
}

const PreProcess_Spawn = ( spawn_in, config_in ) => {
  const spawn_in_at_each             = spawn_in.filter( spawn => spawn.hasOwnProperty('AtEach') );
  const spawn_in_at_group_expression = spawn_in.filter( spawn => spawn.hasOwnProperty('AtGroup') && spawn.hasOwnProperty('Expression') );

  return {
    SpawnAtEach:            spawn_in_at_each.map( spawn => PreProcess_Spawn_AtEach( spawn, config_in ) ),
    SpawnAtGroupExpression: spawn_in_at_group_expression.map( spawn => PreProcess_Spawn_AtGroup_Expression( spawn, config_in ) ),
  }
}

const PreProcess = ( config_in ) => {

  //
  // Pass-1
  //

  const preprocessor = {
    PlayArea:         value => { return { PlayArea: value } },
    MaxLocationCount: value => { return { MaxLocationCount: value } },
    Location:         value => { return { Location: value } },  
    BaseSize:         value => { return { BaseSize: value } }, 
    BaseSpeed:        value => { return { BaseSpeed: value } }, 
    Spawn:            PreProcess_Spawn,
  };
  config_out = {};
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

  if ( config_out.hasOwnProperty('SpawnAtGroupExpression') ) {
    let text_u8 = [];
    config_out.SpawnAtGroupExpression.forEach( spawn => {
      const pattern = [ ...spawn.Pattern ];
      delete spawn.Pattern;
                
      const text_u8_str = text_u8.join('');
      const pattern_str  = pattern.join('');
      let pattern_index = text_u8_str.indexOf( pattern_str );

      if ( pattern_index == -1 ) {
        pattern_index = text_u8.length;
        text_u8       = [ ...text_u8, ...pattern ];
      } else {
        pattern_index = pattern_index / 4; // "0x--"
      }

      spawn.PatternLength = pattern.length;
      spawn.PatternTextU8Offset = pattern_index;
    });
    config_out.TextU8 = text_u8;
  }
  if ( config_out.hasOwnProperty('SpawnAtEach') ) {
    let text_u32 = [];
    config_out.SpawnAtEach.forEach( spawn => {
      const pattern = [ ...spawn.Pattern ];
      delete spawn.Pattern;

      const text_u32_str = text_u32.join('');
      const pattern_str  = pattern.join('');
      let pattern_index = text_u32_str.indexOf( pattern_str );
 
      if ( pattern_index == -1 ) {
        pattern_index = text_u32.length;
        text_u32      = [ ...text_u32, ...pattern ];
      } else {
        pattern_index = pattern_index / 10; // "0x--------"
      }

      spawn.PatternTextU32Offset = pattern_index;
    });
    config_out.TextU32 = text_u32;
  }

  return config_out;
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

fs.writeFileSync( preprocess_filename_out, yaml.dump(config) );
