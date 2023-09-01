const fs   = require('fs');
const vm   = require('vm');
const yaml = require('js-yaml');
const ejs  = require('ejs');
const path = require('path');
const assert = require('assert');
const textEncoder = new TextEncoder();

let debug_context = "";

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

const LoadTypes = ( file_name ) => {
  try {
    let type_info  = yaml.load( fs.readFileSync( file_name, 'utf8' ) );
    // only type_info are included. 
    if (type_info.hasOwnProperty('include')) {
      const include_files = type_info.include.map( include => include.file );
      include_files.forEach( include_file_name => {
        type_info.types = { ...type_info.types, ...LoadTypes( include_file_name ).types };
      });
    }
    return type_info;
  } catch (e) {
    assert.fail('LoadTypes: ' + file_name + '\n' + e.toString() );
  }
}

const LoadValues = ( file_name ) => {
  try {
    let values = yaml.load( fs.readFileSync( file_name, 'utf8' ) );
    return values;
  } catch (e) {
    assert.fail('LoadValues: ' + file_name + '\n' + e.message );
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const EvaluateExpressionText = ( expression_text, source ) => {
  const context = { ...source };
  vm.createContext( context );

  if (typeof(expression_text) == 'object') {
    const expression_object = expression_text;
    const result = {};
    Object.keys(expression_object).forEach( key => {
      const key_expression_text = expression_object[key];
      vm.runInContext( "var _result = " + key_expression_text, context );
      result[key] = context._result;
    });
    return result;
  }

  try {
    vm.runInContext( "var _result = " + expression_text, context );
  } catch(e) {
    assert.fail('EvaluateExpresisonText: ' + JSON.stringify(expression_text,null,2) + '\n' + e.message + '\n' + 'context: ' + JSON.stringify(context, null, 2) ); 
  }
  return context._result;
}


const MapValues = ( type_info, source, input_type_name, output_type_name ) => {
  if ( input_type_name == output_type_name ) {
    return { ...source };
  }

  assert( type_info.types.hasOwnProperty( input_type_name ), `No definition for type: ${input_type_name}` );
  assert( type_info.types.hasOwnProperty( output_type_name ), `No definition for type: ${output_type_name}` );

  let output = { };

  const output_type = type_info.types[ output_type_name ];
  const input_type  = type_info.types[ input_type_name ];

  assert( input_type.hasOwnProperty( 'map-to' ), `No map-to definitions from type: ${input_type_name} to ${output_type_name}` );
  assert( input_type['map-to'].hasOwnProperty( output_type_name ), `Missing map-to for type: ${output_type_name} in type: ${input_type_name}` );

  const value_map = input_type['map-to'][output_type_name];

  output_type.seq.forEach( seq => {
    if ( value_map.hasOwnProperty(seq.id) ) {

      // if map is 1:1 with input, map that type.
      const input_value_seq_id    = value_map[seq.id];
      const input_value_seq_index = input_type.seq.findIndex( seq => seq.id == input_value_seq_id );
      if ( input_value_seq_index != -1) {
        const input_value_seq = input_type.seq[ input_value_seq_index ];

        if ( input_value_seq.hasOwnProperty('type') ) {
          const input_value_seq_type_name = input_value_seq.type;

          if (seq.hasOwnProperty('type')) {
            const output_value_seq_type_name = seq.type;
            const input_value_seq_source     = source[ input_value_seq_id ];
            const value                      = MapValues(type_info, input_value_seq_source, input_value_seq_type_name, output_value_seq_type_name);
            output[ seq.id ] = value;
          }
        }
      } else {
        // evaluate expression to assign value
         
        const value_map_text        = value_map[seq.id];
        const value                 = EvaluateExpressionText( value_map_text, source );
        output[ seq.id ] = value;
      }
    }

  });

  return output;
}

const BuildKsyTypeName = ( type_info, type_name ) => {
  let ksy_type_name = type_name;
  switch (type_name) {
    case "u8":
      ksy_type_name = "u1";
      break;
    case "u16":
      ksy_type_name = "u2";
      break;
    case "u32":
      ksy_type_name = "u4";
      break;
    case "u64":
      ksy_type_name = "u8";
      break;

    case "s8":
      ksy_type_name = "s1";
      break;
    case "s16":
      ksy_type_name = "s2";
      break;
    case "s32":
      ksy_type_name = "s4";
      break;
    case "s64":
      ksy_type_name = "s8";
      break;

    case "f32":
      ksy_type_name = "f4";
      break;
    case "f64":
      ksy_type_name = "f8";
      break;

    case "utf8":
    {
      ksy_type_name = "u4"; // offset
    }
    break;
  }

  return ksy_type_name;
}

const BuildKsyTypeSeqArray = ( type_info, type, seq, ksy_type_references ) => {
  const seq_id            = seq['id'];
  const type_name         = seq['array-of'];
  const count             = seq['count'];
  const ksy_offset_seq_id = seq_id + "_offset";
  const ksy_value_seq_id  = seq_id + "_value";

  const ksy_seq = { 
    id: ksy_offset_seq_id,
    type: "u4",
  };

  const ksy_type_name     = BuildKsyTypeName( type_info, type_name );

  let ksy_instance = {
    type: ksy_type_name,
    pos: ksy_offset_seq_id,
  };

  if ( count != null ) {
    const repeat = {
      repeat: 'expr',
      'repeat-expr': count,
    };
    ksy_instance = { ...ksy_instance, ...repeat }; 
  } else {
    const repeat = {
      repeat: 'until',
      'repeat-until': '_ == 0',
    };
    ksy_instance = { ...ksy_instance, ...repeat }; 
  }

  ksy_type_references[ ksy_value_seq_id ] = ksy_instance;

  return ksy_seq;
}

const BuildKsyTypeSeqValue = ( type_info, type, seq, ksy_type_references ) => {
  const seq_id        = seq['id'];
  const type_name     = seq['type'];
  let   ksy_seq_id    = seq_id;
  let   ksy_type_name = type_name;

  switch (type_name) {
    case "u8":
      ksy_type_name = "u1";
      break;
    case "u16":
      ksy_type_name = "u2";
      break;
    case "u32":
      ksy_type_name = "u4";
      break;
    case "u64":
      ksy_type_name = "u8";
      break;

    case "s8":
      ksy_type_name = "s1";
      break;
    case "s16":
      ksy_type_name = "s2";
      break;
    case "s32":
      ksy_type_name = "s4";
      break;
    case "s64":
      ksy_type_name = "s8";
      break;

    case "f32":
      ksy_type_name = "f4";
      break;
    case "f64":
      ksy_type_name = "f8";
      break;

    case "utf8":
    {
      ksy_seq_id    = seq_id + "_offset";
      ksy_type_name = "u4"; // offset

      const ksy_instance_name = seq_id + "_value";
      const ksy_instance = {
        type: "str",
        encoding: "UTF-8",
        pos: ksy_seq_id,
        terminator: 0,
      };

      ksy_type_references[ ksy_instance_name ] = ksy_instance;
    }
    break;
  }

  const ksy_seq = { 
    id:   ksy_seq_id,
    type: ksy_type_name,
  };

  return ksy_seq;
}

const BuildKsyTypeSeq = ( type_info, type, seq, ksy_type_references ) => {
  const is_array  = seq.hasOwnProperty('array-of');
  if ( is_array ) {
    return BuildKsyTypeSeqArray( type_info, type, seq, ksy_type_references );
  }
  return BuildKsyTypeSeqValue( type_info, type, seq, ksy_type_references );
}

const BuildKsyType = ( type_info, type_name ) => {
  assert( type_info.types.hasOwnProperty( type_name ), `No definition for type: ${type_name}` );

  const type     = type_info.types[ type_name ];
  const ksy_type = { seq: [] };
  const ksy_type_references = {};

  type.seq.forEach( seq => {
    debug_context += " " + seq.id;
    const ksy_seq = BuildKsyTypeSeq( type_info, type, seq, ksy_type_references );
    ksy_type.seq.push( ksy_seq );
  });
  if ( Object.keys(ksy_type_references).length ) {
    ksy_type.instances = ksy_type_references;
  }

  return ksy_type;
}

const BuildKsy = ( type_info, type_name ) => {
  debug_context += " build_ksy";

  let ksy  = {};

  ksy['meta']      = { id: type_name, endian: 'le' };
  ksy['seq']       = [];
  ksy['types']     = {};

  const ksy_type = BuildKsyType( type_info, type_name );
  ksy = { ...ksy, ...ksy_type };

  Object.keys(type_info.types).forEach( type_name => {
    debug_context += " " + type_name;
    const ksy_type = BuildKsyType( type_info, type_name );
    ksy.types[ type_name ] = ksy_type;
  });

  return ksy;
}

const WriteKsy = ( type_info, type_name, output_file_name ) => {
  debug_context = "write_ksy";

  const ksy = BuildKsy( type_info, type_name );
  fs.writeFileSync( output_file_name, yaml.dump( ksy, { condenseFlow: true, noRefs: true, noCompatMode:  true } ) );
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 6) {
  console.error( process.argv[1] + ' [type_info.yml] [input_type] [output_type=ksy] [output.ksy]' );
  process.exit(1);
}

const type_info_file_name = process.argv[2];
const input_type_name     = process.argv[3];
const output_file_type    = process.argv[4];
const output_file_name    = process.argv[5];
const type_info           = LoadTypes( type_info_file_name );

if (output_file_type == "ksy") {
  WriteKsy( type_info, input_type_name, output_file_name );
  console.log(`Wrote: ${output_file_name} as ${output_file_type}`);
}

