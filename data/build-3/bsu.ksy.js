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

const WriteMagicSeq = ( ksy, schema ) => {
  ksy.seq.push( { id: 'magic', type: 'str', encoding: 'UTF-8', size: 64 } );
}

const WriteMutableRef = ( ksy, schema ) => {
  ksy.types["mutable"] = {
    seq: [
      { id: 'offset', type: 'u4' },
      { id: 'size', type: 'u4' },
    ],
    instances: {
      value: {
        type: 'u1',
        pos: 'offset',
        repeat: 'expr',
        'repeat-expr': 'size',
      }
    }
  };
  ksy.seq.push( { id: 'mutable', type: 'mutable' } );
}

const WriteSectionsSeq = ( ksy, schema ) => {
  const toc_ksy = {seq:[]};
  schema.sections.forEach( section => {
    const section_gid       = section.id;
    const section_offset_id = `${section_gid}_offset`;
    toc_ksy.seq.push( { id: section_offset_id, type: 'u4' } );
  });
  ksy.types['struct_toc'] = toc_ksy;
  ksy.seq.push( { id: 'toc', type: 'struct_toc' } );
}

const WriteStaticArray = ( ksy, element_id, schema ) => {
  const static_array_ksy_id = `static_array_${element_id}`;
  if ( ksy.types.hasOwnProperty(static_array_ksy_id) ) {
    return;
  }
  if ( element_id == 'utf8' ) {
    element_id = 'string_utf8';
  }
  const static_array_ksy = { 
    seq: [ 
      { id: 'offset', type: 'u4' }, 
      { id: 'count', type: 'u4' } 
    ],
    instances: {
      value: {
        type: element_id,
        pos: 'offset',
        repeat: 'expr',
        'repeat-expr': 'count',
      }
    } 
  };

  ksy.types[static_array_ksy_id] = static_array_ksy;

  if ( element_id == 'string_utf8' ) {
    if (!ksy.types.hasOwnProperty('string_utf9')) {
      const string_utf8_ksy = {
        seq: [
          { id: 'offset', type: 'u4' }
        ],
        instances: {
          value: {
            type: 'str',
            encoding: 'UTF-8',
            pos: 'offset',
            terminator: 0,
          } 
        }
      };
      ksy.types['string_utf8'] = string_utf8_ksy;
    }
  }
}

const ConvertStep = ( ksy, step ) => {
    let instance = { };
    const step_type = step.type;
    let step_ksy_type = step_type;
    if (step_type == 'u32') {
      step_ksy_type = 'u4';
    } else if (step_type == 'f32') {
      step_ksy_type = 'f4';
    } else if (step_type == 'u8') {
      step_ksy_type = 'u1';
    } else if (step_type == 'u16') {
      step_ksy_type = 'u2';
    }

    if ( step.container == 'static_array' ) {
      const static_array_ksy_type = `static_array_${step_ksy_type}`;
      instance['type'] = static_array_ksy_type;
      WriteStaticArray( ksy, step_ksy_type, schema );
    } else {
      instance['type'] = step_ksy_type;
    }
  return instance;
}

const WriteSectionsInstances = ( ksy, schema ) => {
  schema.sections.forEach( section => {
    const section_gid        = section.id;
    const section_offset_id = `_root.toc.${section_gid}_offset`;
    ksy.instances[section_gid] = { pos: section_offset_id, ...ConvertStep(ksy, section) };
  });
}

const WriteTypes = ( ksy, schema ) => {
  Object.keys(schema.types).forEach( type_id => {
    const type_seq = schema.types[type_id];
    const type_ksy = {seq:[]};
    type_seq.forEach( step => {
      type_ksy.seq.push( { id: step.id, ...ConvertStep(ksy, step) } );
    });
    ksy.types[type_id] = type_ksy;
  });
}

const WriteKsy = ( schema, ksy_filename_out ) => {
  const ksy  = {};

  ksy['meta'] = { id: 'bsu_file', endian: 'le' };
  ksy['seq']  = [];
  ksy['instances'] = {};
  ksy['types'] = {};

  WriteMagicSeq( ksy, schema );
  WriteMutableRef( ksy, schema );
  WriteSectionsSeq( ksy, schema );
  WriteSectionsInstances( ksy, schema );
  WriteTypes( ksy, schema );

  fs.writeFileSync( ksy_filename_out, yaml.dump(ksy) );
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 4) {
  console.error( process.argv[1] + ' [ksy_schema.yml] [output_filename]' );
  process.exit(1);
}

const schema_filename_in           = process.argv[2];
const ksy_filename_out             = process.argv[3];
const schema                       = yaml.load( fs.readFileSync( schema_filename_in, 'utf8' ) );

WriteKsy( schema, ksy_filename_out );
