
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
    return    "      - \"" + sample_trigger + "\"";
  }).join("\n");;
}

const AverageSpawnTime = (wave) => {
  if (!wave) return 0.0;
  if (wave.length == 1) return 0.0;
  const sum = wave.reduce( ( acc, sample, index ) => {
    if ( index == 0 ) {
      return 0.0;
    }
    const n = wave[ index ][0];
    const p = wave[ index-1 ][0];
    const d = n-p;
    return acc+d;
  }, 0.0);
  const count = wave.length-1;
  const avg = sum/count;
  const quantized_avg = ((avg * 2.0)|0) * 0.5;
  return quantized_avg;
}

const SpawnTimeOffset = (wave) => {
  if (!wave) return 0.0;
  const offset = wave[0][0] + 0.4999;
  const quantized_offset = ((offset * 2.0)|0) * 0.5;
  return quantized_offset;
}

const SpawnTimeEnd = (wave) => {
  if (!wave) return 0.0;
  const offset = wave[wave.length-1][0] + 0.4999;
  const quantized_offset = ((offset * 2.0)|0) * 0.5;
  return quantized_offset;
}

const ConfigItemToString = (item_name) => {
  const item = config[ item_name ];
  let output = "";
  output += item_name + ":\n";
  output += "  MaxCount:     "  + item.MaxCount + "\n";
  output += "  BaseSize:     [ " + item.BaseSize[0].toFixed(2) + ", " + item.BaseSize[1].toFixed(2) + " ]\n";
  output += "  BaseVelocity: [ " + item.BaseVelocity[0].toFixed(2) + ", " + item.BaseVelocity[1].toFixed(2) + " ]\n";

  const time_step_1 = AverageSpawnTime( item["Level_1"] );
  const time_step_2 = AverageSpawnTime( item["Level_2"] );
  const time_step_3 = AverageSpawnTime( item["Level_3"] );
  const time_step_4 = AverageSpawnTime( item["Level_4"] );

  const time_end_1 = SpawnTimeEnd( item["Level_1"] );
  const time_end_2 = SpawnTimeEnd( item["Level_2"] );
  const time_end_3 = SpawnTimeEnd( item["Level_3"] );
  const time_end_4 = SpawnTimeEnd( item["Level_4"] );

  const time_offset_1 = SpawnTimeOffset( item["Level_1"] );
  const time_offset_2 = SpawnTimeOffset( item["Level_2"] );
  const time_offset_3 = SpawnTimeOffset( item["Level_3"] );
  const time_offset_4 = SpawnTimeOffset( item["Level_4"] );

  if (item.hasOwnProperty("Level_1")) {
    output += "  Level_1:\n";
    output += "    SpawnTimeStep:   " + time_step_1.toFixed(2) + "\n";
    output += "    SpawnTimeOffset: " + time_offset_1.toFixed(2) + "\n";
    output += "    SpawnTimeEnd:    " + time_end_1.toFixed(2) + "\n";
    output += "    SpawnPattern:\n";
    output += ConfigWaveToString( item["Level_1"] ) + "\n";
  }
  if (item.hasOwnProperty("Level_2")) {
    output += "  Level_2:\n";
    output += "    SpawnTimeStep:   " + time_step_2.toFixed(2) + "\n";
    output += "    SpawnTimeOffset: " + time_offset_2.toFixed(2) + "\n";
    output += "    SpawnTimeEnd:    " + time_end_2.toFixed(2) + "\n";
    output += "    SpawnPattern:\n";
    output += ConfigWaveToString( item["Level_2"] ) + "\n";
  }
  if (item.hasOwnProperty("Level_3")) {
    output += "  Level_3:\n";
    output += "    SpawnTimeStep:   " + time_step_3.toFixed(2) + "\n";
    output += "    SpawnTimeOffset: " + time_offset_3.toFixed(2) + "\n";
    output += "    SpawnTimeEnd:    " + time_end_3.toFixed(2) + "\n";
    output += "    SpawnPattern:\n";
    output += ConfigWaveToString( item["Level_3"] ) + "\n";
  }
  if (item.hasOwnProperty("Level_4")) {
    output += "  Level_4:\n";
    output += "    SpawnTimeStep:   " + time_step_4.toFixed(2) + "\n";
    output += "    SpawnTimeOffset: " + time_offset_4.toFixed(2) + "\n";
    output += "    SpawnTimeEnd:    " + time_end_4.toFixed(2) + "\n";
    output += "    SpawnPattern:\n";
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
