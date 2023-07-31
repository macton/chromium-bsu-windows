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

const snakeToPascalCase = (inputString) => {
  const words = inputString.split('_');
  for (let i = 0; i < words.length; i++) {
    words[i] = words[i].charAt(0).toUpperCase() + words[i].slice(1);
  }
  const pascalCaseString = words.join('');
  return pascalCaseString;
}


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

const WriteMagicSeq = ( hpp, schema ) => {
  hpp.addr += 64;
}

const WriteSectionsSeq = ( hpp, schema ) => {
  schema.sections.forEach( section => {
    const section_gid       = section.id;
    const section_offset_id = `${section_gid}_offset`;
    const constant_name     = 'k' + snakeToPascalCase(section_offset_id);
    hpp.map[ constant_name ] = hpp.addr;
    hpp.addr += 4;
  });
}

const WriteMutableSeq = ( hpp, schema ) => {
  hpp.map[ 'kMutableOffset' ] = hpp.addr;
  hpp.addr += 4;
  hpp.map[ 'kMutableSize' ] = hpp.addr;
  hpp.addr += 4;
}

const WriteStaticArray = ( namespace, hpp, element_id, schema ) => {

  const offset_id = namespace + "_offset";
  const count_id  = namespace + "_count";

  hpp.map[ offset_id ] = hpp.addr;
  hpp.addr += 4;

  hpp.map[ count_id ] = hpp.addr;
  hpp.addr += 4;
}

const ConvertStep = ( namespace, hpp, step, schema ) => {
  if ( step.container == 'static_array' ) {
    const step_id = namespace + "_static_array";
    WriteStaticArray( step_id, hpp, step.type, schema );
  } else if ( step.type == 'u32' ) {
    const step_id = namespace + "_" + step.type;
    hpp.map[ step_id ] = hpp.addr;
    hpp.addr += 4;
  } else if ( step.type == 'u8' ) {
    const step_id = namespace + "_" + step.type;
    hpp.map[ step_id ] = hpp.addr;
    hpp.addr += 1;
  } else if ( step.type == 'f32' ) {
    const step_id = namespace + "_" + step.type;
    hpp.map[ step_id ] = hpp.addr;
    hpp.addr += 4;
  } else {
    const step_id = namespace + "_" + step.type;
    WriteType( namespace, hpp, schema.types[step.type], schema );
  }
}

const WriteSectionsInstances = ( hpp, schema ) => {
  schema.sections.forEach( section => {
    const section_gid        = section.id;
    const section_offset_id = `${section_gid}_offset`;

    hpp.addr = hpp.map[ section_offset_id ];
    ConvertStep(section_gid, hpp, section, schema);
  });
}

const WriteType = ( namespace, hpp, type_seq, schema ) => {
  type_seq.forEach( step => {
    const step_id = namespace + "_" + step.id;
    ConvertStep( step_id, hpp, step, schema );
  });
}

const WriteTypes = ( hpp, schema ) => {
  Object.keys(schema.types).forEach( type_id => {
    const type_seq = schema.types[type_id];
    const skip_type = ((type_seq.length == 1) && ( type_seq[0].container == 'static_array' ));
    if (!skip_type) {
      type = [];
      type_seq.forEach( step => {
        const field_name = step.id;
        let field_type = step.type;
        if ( step.container == 'static_array' ) {
          field_type = 'static_array'; 
        } else if ( step.type == 'u32' ) {
          field_type = 'uint32_t'; 
        } else if ( step.type == 'u16' ) {
          field_type = 'uint16_t'; 
        } else if ( step.type == 'f32' ) {
          field_type = 'float'; 
        } else if ( step.type == 'f32' ) {
          field_type = 'float'; 
        }
        type.push( { type: field_type, name: field_name } );
      });
      hpp.types[ type_id ] = type;
    } 
  });
}

const WriteHpp = ( schema, hpp_filename_out ) => {
  const hpp  = {
    addr: 0,
    map: {},
    types: {},
  };

  WriteMagicSeq( hpp, schema );
  WriteMutableSeq( hpp, schema );
  WriteSectionsSeq( hpp, schema );
  WriteTypes( hpp, schema );

  const file = fs.openSync( hpp_filename_out, 'w' );
  fs.writeSync(file,'#pragma once\n');
  fs.writeSync(file,'#include <stdint.h>\n');
  fs.writeSync(file,'\n');

  // offset map
  fs.writeSync(file,'// Offset Map\n');
  Object.keys(hpp.map).forEach( constant_name => {
    const constant_value = hpp.map[constant_name]; 
    fs.writeSync(file,`#define ${constant_name.padEnd(30,' ')} ${constant_value}\n`);
  });
  fs.writeSync(file,'\n');

  // named constants
  if (schema.hasOwnProperty('constants')) {
    fs.writeSync(file,'// Named Constants\n');
    schema.constants.forEach( constant => {
      const constant_name    = constant.id;
      const constant_type    = constant.type;
      const constant_value   = constant.value;
      if ( constant_type == 'f32' ) {
        fs.writeSync(file,`#define ${constant_name.padEnd(30,' ')} ${constant_value.toFixed(8)}f\n`);
      } else {
        fs.writeSync(file,`#define ${constant_name.padEnd(30,' ')} ${constant_value}\n`);
      }
    });
  }
  fs.writeSync(file,'\n');

  // find text alignment size for struct names
  let struct_name_alignment_size = Object.keys(hpp.types).reduce( (acc, struct_name) => { return Math.max(struct_name.length, acc); }, 0 ) + 4;

  // struct typedefs
  fs.writeSync(file,'#ifndef __cplusplus\n');
  fs.writeSync(file,`typedef struct ${'static_array'.padEnd(struct_name_alignment_size,' ')} static_array;\n`);
  Object.keys(hpp.types).forEach( struct_name => {
    const struct = hpp.types[struct_name];
    fs.writeSync(file,`typedef struct ${struct_name.padEnd(struct_name_alignment_size,' ')} ${struct_name};\n`);
  });
  fs.writeSync(file,'#endif // __cplusplus\n');
  fs.writeSync(file,`\n`);

  // struct definitions
  fs.writeSync(file,`
struct static_array
{
  uint32_t    offset;
  uint32_t    count;
};\n\n`);
  Object.keys(hpp.types).forEach( struct_name => {
    const struct = hpp.types[struct_name];
    let   field_type_alignment_size = struct.reduce( (acc, field) => { return Math.max(field.type.length, acc); }, 0 ) + 1;
    fs.writeSync(file,`struct ${struct_name}\n`);
    fs.writeSync(file,`{\n`);
    struct.forEach( field => {
      fs.writeSync(file,`  ${field.type.padEnd(field_type_alignment_size,' ')} ${field.name};\n`);
    });
    fs.writeSync(file,`};\n`);
    fs.writeSync(file,'\n');
  });

  fs.closeSync(file);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 4) {
  console.error( process.argv[1] + ' [hpp_schema.yml] [output_filename]' );
  process.exit(1);
}

const schema_filename_in           = process.argv[2];
const hpp_filename_out             = process.argv[3];
const schema                       = yaml.load( fs.readFileSync( schema_filename_in, 'utf8' ) );

WriteHpp( schema, hpp_filename_out );
