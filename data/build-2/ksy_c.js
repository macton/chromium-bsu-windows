
const fs   = require('fs');
const vm   = require('vm');
const yaml = require('js-yaml');
const ejs  = require('ejs');
const path = require('path');
const textEncoder = new TextEncoder();

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

// --------------------------------------------------------------------------------

const OutputRead( 

// --------------------------------------------------------------------------------

const ConvertBlockWithType = ( namespace, block, pos ) => {
  const type = block.type;
  if ( type == 'u4' ) {
    return 4;
  }
}

const ConvertBlockWithoutType = ( namespace, block, pos ) => {
}

const ConvertBlock = ( namespace, block, pos ) => {
  if ( block.hasOwnProperty('type') ) {
    return ConvertBlockWithType( namespace, block, pos );
  }
  return ConvertBlockWithoutType( namespace, block, pos );
}

const ConvertSeq = ( namespace, seq, pos ) => {
  let size = 0;
  ksy.seq.forEach( block => {
    size += ConvertBlock( namespace, block, pos+size );
  });
  return size;
}

const Convert = (namespace, ksy) => {
  ConvertSeq( namesapce, ksy.seq, 0 );
}



// --------------------------------------------------------------------------------

if (process.argv.length < 3) {
  console.error( process.argv[1] + ' [input file]' );
  process.exit(1);
}

const filename_in             = process.argv[2];
const ksy                     = yaml.load( fs.readFileSync( filename_in, 'utf8' ) );

Convert(["bsu"], ksy);
