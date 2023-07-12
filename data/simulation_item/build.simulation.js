const fs   = require('fs');
const yaml = require('js-yaml');
const vm   = require('vm');
const ejs  = require('ejs');

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

const RenderItemTemplates = ( item_type, template_filename, config, output_dir, output_extension ) => {
  const template = fs.readFileSync( template_filename, 'utf8' );
  const item_names   = Object.keys(config[item_type]);
  item_names.forEach( item_name => {
    const filename_out = output_dir + '/' + item_name + output_extension;
    const item    = config[item_type][item_name];
    item.Name = item_name;
    const context = {
      Item: item,
      Levels: config.Levels,
      SpawnExpression: config.SpawnExpression,
      SpawnFormation: config.SpawnFormation,
    };

    const text = ejs.render( template, context );
    fs.writeFileSync( filename_out, text );
  });
}

const RenderConfigTemplate = (template_filename, config, output_dir, base_filename_out ) => {
  const template     = fs.readFileSync( template_filename, 'utf8' );
  const filename_out = output_dir + '/' + base_filename_out;
  const context      = {
    Levels: config.Levels,
    SpawnExpression: config.SpawnExpression,
    SpawnFormation: config.SpawnFormation,
  };

  const text = ejs.render( template, context );
  fs.writeFileSync( filename_out, text );
}

if (process.argv.length < 3) {
  console.error( process.argv[1] + ' [input file]' );
  process.exit(1);
}

const filename_in  = process.argv[2];
const config       = yaml.load( fs.readFileSync( filename_in, 'utf8' ) );

RenderConfigTemplate( 'formation.cpp.template', config, '../../generated_src', 'Formation.cpp' );
RenderConfigTemplate( 'formation.hpp.template', config, '../../generated_src', 'Formation.hpp' );
RenderItemTemplates( 'Enemy', 'live.cpp.template', config, '../../generated_src', '_Live.cpp' );
RenderItemTemplates( 'Enemy', 'live.hpp.template', config, '../../generated_src', '_Live.hpp' );
RenderItemTemplates( 'PowerUp', 'live.cpp.template', config, '../../generated_src', '_Live.cpp' );
RenderItemTemplates( 'PowerUp', 'live.hpp.template', config, '../../generated_src', '_Live.hpp' );
RenderItemTemplates( 'EnemyAmmo', 'enemy.ammo.cpp.template', config, '../../generated_src', '_Live.cpp' );
RenderItemTemplates( 'EnemyAmmo', 'live.hpp.template', config, '../../generated_src', '_Live.hpp' );

