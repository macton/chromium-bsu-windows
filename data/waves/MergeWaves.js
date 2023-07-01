
const fs   = require('fs');
const yaml = require('js-yaml');

let config = { };

const LoadWave = ( level, filename_in ) => {
  const level_waves = yaml.load( fs.readFileSync( filename_in, 'utf8' ) );
  const item_names  = Object.keys( level_waves );

  item_names.forEach( item_name => {
    if (!config.hasOwnProperty(item_name)) {
      config[item_name] =  {
         MaxCount: 8,
         BaseSize: [ 0.5, 0.5 ],
         BaseVelocity: [ 0.0, -0.05 ],
      };
    }
    const item_wave = level_waves[ item_name ];
    config[item_name]["Level_"+ level] = item_wave;
  });
}

const ConfigWaveToString = (wave) => {
  return wave.map( sample => {
    const sample_time    = sample[0];
    const sample_trigger = sample[1];

    return "    - [ " + sample_time.toFixed(4).padStart(10," ") + ", \"" + sample_trigger + "\" ]";
  }).join("\n");;
}

const ConfigItemToString = (item_name) => {
  const item = config[ item_name ];
  let output = "";
  output += item_name + ":\n";
  output += "  MaxCount:     "  + item.MaxCount + "\n";
  output += "  BaseSize:     [ " + item.BaseSize[0].toFixed(2) + ", " + item.BaseSize[1].toFixed(2) + " ]\n";
  output += "  BaseVelocity: [ " + item.BaseVelocity[0].toFixed(2) + ", " + item.BaseVelocity[1].toFixed(2) + " ]\n";
  if (item.hasOwnProperty("Level_1")) {
    output += "  Level_1:\n";
    output += ConfigWaveToString( item["Level_1"] ) + "\n";
  }
  if (item.hasOwnProperty("Level_2")) {
    output += "  Level_2:\n";
    output += ConfigWaveToString( item["Level_2"] ) + "\n";
  }
  if (item.hasOwnProperty("Level_3")) {
    output += "  Level_3:\n";
    output += ConfigWaveToString( item["Level_3"] ) + "\n";
  }
  if (item.hasOwnProperty("Level_4")) {
    output += "  Level_4:\n";
    output += ConfigWaveToString( item["Level_4"] ) + "\n";
  }
  return output + "\n";
}

const ConfigToString = () => {
  let output = "";

  const item_names  = Object.keys( config );
  item_names.forEach( item_name => output += ConfigItemToString( item_name ) );

  return output;
}

LoadWave(1,"waves_1.txt");
LoadWave(2,"waves_2.txt");
LoadWave(3,"waves_3.txt");
LoadWave(4,"waves_4.txt");

fs.writeFileSync( "simulation_item_config.yml", ConfigToString() );
