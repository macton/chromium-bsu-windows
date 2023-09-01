const fs   = require('fs');
const vm   = require('vm');
const yaml = require('js-yaml');
const ejs  = require('ejs');
const path = require('path');
const assert = require('assert');
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

const CreateDataSection = () =>
{
  return {
    start_offset: 0,
    size: 0,
    offsets: {},
    block: [],
  };
}

const WriteSeqArray = ( gid, seq_value, seq, type_info, reference_sections, output_file ) => {
  let   size       = 0;
  const type_name  = seq['array-of'];
   
  if ( !reference_sections.hasOwnProperty(type_name) ) {
    reference_sections[type_name] = CreateDataSection();
  }
  const type_section     = reference_sections[type_name];
  const reference_gid    = gid + '.data';
  const reference_offset = type_section.offsets[reference_gid] || 0;

  size += WriteU32( reference_offset, output_file );

  if (seq_value) {
    type_section.block.push( {
      gid: reference_gid,
      write: () => {
        let size = 0;
        seq_value.forEach( (element_value, element_index) => {
          const element_gid = reference_gid + "[" + element_index + "]";
          const element_seq = { type: type_name };
          size += WriteSeq( element_gid, element_value, element_seq, type_info, reference_sections, output_file );
        });
        return size;
      },
    });
  }

  return size;
}

const WriteSeqValue = ( gid, seq_value, seq, type_info, reference_sections, output_file ) => {
  let   size      = 0;
  const type_name = seq.type;

  if ( type_name == "u32" ) {
    size += WriteU32( seq_value || 0, output_file );
  } else if ( type_name == "u8" ) {
    size += WriteU8( seq_value || 0, output_file );
  } else if ( type_name == "u16" ) {
    size += WriteU16( seq_value || 0, output_file );
  } else if ( type_name == "f32" ) {
    size += WriteF32( seq_value || 0.0, output_file );
  } else if ( type_name == "utf8" ) {

    if ( !reference_sections.hasOwnProperty('utf8') ) {
      reference_sections['utf8'] = CreateDataSection();
    }
    const utf8_gid     = gid + '.utf8';
    const utf8_section = reference_sections['utf8'];
    const utf8_offset  = utf8_section.offsets[utf8_gid] || 0;

    size += WriteU32( utf8_offset, output_file );

    utf8_section.block.push( {
      gid: utf8_gid,
      write: () => {
        let size = 0; 
        size += WriteUTF8( seq_value, output_file );
        return size;
      },
    });

  } 
  else {
    assert( type_info.types.hasOwnProperty(type_name), `${gid}: Missing type info for ${type_name}` );
    const type      = type_info.types[type_name];
    const seq_count = type.seq.length;

    for (let i=0;i<seq_count;i++) {

      const field_seq   = type.seq[i];
      const field_id    = field_seq.id;
      const field_gid   = gid + "." + field_id;
      const field_value = seq_value && seq_value[ field_id ] || null;

      size += WriteSeq( field_gid, field_value, field_seq, type_info, reference_sections, output_file );
    }
  }

  return size;
}

const WriteSeq = ( gid, seq_value, seq, type_info, reference_sections, output_file ) => {
  const is_array  = seq.hasOwnProperty('array-of');

  if ( is_array ) {
    return WriteSeqArray( gid, seq_value, seq, type_info, reference_sections, output_file );
  }
  return WriteSeqValue( gid, seq_value, seq, type_info, reference_sections, output_file );
}

const WriteBinFile = ( source, type_name, type_info, reference_sections, output_file_name ) => {

  const output_file = fs.openSync( output_file_name, "w" );
  let   size       = 0;

  type_info.types[ type_name ].seq.forEach( seq => {
    const seq_id    = seq.id;
    const seq_value = source[seq_id] || null;
    const seq_gid   = seq_id;
    size += WriteSeq( seq_gid, seq_value, seq, type_info, reference_sections, output_file );
  });

  Object.keys( reference_sections ).sort().forEach( data_section_name => {
    const data_section = reference_sections[ data_section_name ];
    data_section.start = size;
    if (data_section.block.length > 0) {
      let next_block = data_section.block.shift(); 
      while ( next_block ) {
        const next_gid   = next_block.gid; 
        const next_write = next_block.write;
console.log(`.data ${next_gid} = ${size}`);
        data_section.offsets[next_gid] = size;
        const next_block_size = next_write();
        const next_block_offset = size;
        size += next_block_size;
        next_block = data_section.block.shift(); 
      }
    }
    data_section.size = size - data_section.start;
  });

  fs.closeSync(output_file);
}

const WriteBin = ( source, type_name, type_info, output_file_name ) => {

  const reference_sections = {};

  // Write once to gather all the offsets
  WriteBinFile( source, type_name, type_info, reference_sections, output_file_name );

  // Write again to write the correct offsets
  WriteBinFile( source, type_name, type_info, reference_sections, output_file_name );
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


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 8) {
  console.error( process.argv[1] + ' [type_info.yml] [input_type] [input.yml] [output_file_type: bin|yml] [output_type] [output.bin]' );
  process.exit(1);
}

const type_info_file_name = process.argv[2];
const input_type_name     = process.argv[3];
const input_file_name     = process.argv[4];
const output_file_type    = process.argv[5];
const output_type_name    = process.argv[6];
const output_file_name    = process.argv[7];

const type_info   = LoadTypes( type_info_file_name );
const source      = LoadValues( input_file_name );
const output      = MapValues( type_info, source, input_type_name, output_type_name );

if ( output_file_type == 'yml' ) {
  fs.writeFileSync( output_file_name, yaml.dump( output, { condenseFlow: true, noRefs: true, noCompatMode:  true } ) );
}
else if ( output_file_type == 'bin' ) {
  WriteBin( output, output_type_name, type_info, output_file_name );
}
console.log(`Wrote: ${output_file_name} as ${output_file_type}`);
