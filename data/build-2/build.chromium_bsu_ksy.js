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

const WriteMagic = ( schema, file ) => {
  const seq = `
  - id: magic
    type: str
    encoding: UTF-8
    size: 64
`;
  fs.writeSync( file, seq );
}

const WriteSections = ( schema, file ) => {
  schema.sections.forEach( section => {
    const section_gid    = section.id;
    const seq = `
  - id: ${section_gid}_offset
    type: u4
`;
    fs.writeSync( file, seq );
  });
}

const WriteKsy = ( schema, ksy_filename_out ) => {
  const file = fs.openSync( ksy_filename_out, "w" );
  let   size = 0;

  const meta =  `
meta:
  id: bsu_file
  endian: le
`;
  const seq_start = `
seq:
`;

  fs.writeSync( file, meta );
  fs.writeSync( file, seq_start );
  

  size += WriteMagic( schema, file );
  size += WriteSections( schema, file );

  fs.closeSync(file);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

if (process.argv.length < 3) {
  console.error( process.argv[1] + ' [ksy_schema.yml]' );
  process.exit(1);
}

const schema_filename_in           = process.argv[2];
const schema                       = yaml.load( fs.readFileSync( schema_filename_in, 'utf8' ) );
const schema_filename_in_extname   = path.extname( schema_filename_in );
const schema_filename_in_basename  = path.basename( schema_filename_in, schema_filename_in_extname );
const schema_filename_in_dirname   = path.dirname( schema_filename_in );
const ksy_filename_out             = path.resolve( schema_filename_in_dirname, schema_filename_in_basename + ".ksy" );

WriteKsy( schema, ksy_filename_out );
