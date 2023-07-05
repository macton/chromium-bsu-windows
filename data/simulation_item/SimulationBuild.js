const fs   = require('fs');
const yaml = require('js-yaml');
const vm   = require('vm');

const FLT_MIN = 1.17549435082229e-038;

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


function setCharAt(str,index,chr) {
    if(index > str.length-1) return str;
    return str.substring(0,index) + chr + str.substring(index+1);
}

const ProcessSpawnGeneratedPattern = ( item ) => {
  for (let level_id=1;level_id<=4;level_id++) {
    const level_name = "Level_" + level_id;
    if ( item.hasOwnProperty(level_name) ) {
      const level             = item[level_name];
      const spawn_time_step   = level["SpawnTimeStep"] + FLT_MIN;
      const spawn_time_offset = level["SpawnTimeOffset"];
      const spawn_time_end    = level["SpawnTimeEnd"];
      let   pattern_timeline = [];

      for (let spawn_time=spawn_time_offset;spawn_time<=spawn_time_end;spawn_time+=spawn_time_step) {
        let pattern = "--------------------";
        pattern_timeline.push(pattern);
      }

      if ( level.hasOwnProperty("SpawnExpression") ) {
        const spawn_expression  = level["SpawnExpression"];
        const sandbox = {
          M_PI: 3.14159265358979323846264338327950288,
          sin: Math.sin,
          cos: Math.cos,
        };
        vm.createContext(sandbox);
        for (let spawn_time=spawn_time_offset;spawn_time<=spawn_time_end;spawn_time+=spawn_time_step) {
          const pattern_ndx = ((spawn_time-spawn_time_offset)/spawn_time_step)|0;
          let   pattern     = pattern_timeline[pattern_ndx];

          sandbox.SpawnTime       = spawn_time;
          sandbox.SpawnTimeLength = spawn_time_end-spawn_time_offset;

          spawn_expression.forEach( expression => {
            vm.runInContext( "_result = " + expression, sandbox );
            let result = sandbox._result;
            let result_index = ((result * 10.0) + 10.0)|0;
            result_index = (result_index < 0)?0:result_index;
            result_index = (result_index > 19)?19:result_index;
  
            pattern = setCharAt( pattern, result_index, '*');
          });
          pattern_timeline[pattern_ndx] = pattern;
        }
      }

      if ( level.hasOwnProperty("SpawnFormationStamps") ) {
        const spawn_stamp = level["SpawnFormationStamps"];
        spawn_stamp.forEach( stamp => {
          const stamp_time_start = stamp["TimeStart"];
          const stamp_form_name  = stamp["Formation"];
          const stamp_form       = item["SpawnFormations"][stamp_form_name];
          let   spawn_time       = stamp_time_start;
          stamp_form.forEach( form_pattern => {
            const pattern_ndx = ((spawn_time-spawn_time_offset)/spawn_time_step)|0;
            if ( pattern_ndx < pattern_timeline.length ) {
              let   pattern     = pattern_timeline[pattern_ndx];
              for (let i=0;i<20;i++) {
                if (form_pattern.charAt(i) == '*') {
                  pattern = setCharAt( pattern, i, '*');
                }
                else if (form_pattern.charAt(i) == '.') {
                  pattern = setCharAt( pattern, i, '-');
                }
              }
              spawn_time += spawn_time_step;
              pattern_timeline[pattern_ndx] = pattern;
            }
          });
        });
      }
      level["SpawnPatternGenerated"] = pattern_timeline;
    }
  }
}

const ProcessConfig = ( config ) => {
  const meta = { };
  meta.ItemNames = Object.keys( config );

  meta.ItemNames.forEach( item_name => {
    ProcessSpawnGeneratedPattern( config[item_name] );
  });

  config["_META"] = meta;
  return config;
}

if (process.argv.length < 4) {
  console.error( process.argv[1] + ' [input file] [output file]' );
  process.exit(1);
}

const filename_in  = process.argv[2];
const filename_out = process.argv[3];
const config       = ProcessConfig( yaml.load( fs.readFileSync( filename_in, 'utf8' ) ) );

fs.writeFileSync( filename_out, yaml.dump(config) );
