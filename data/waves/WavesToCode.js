const fs   = require('fs');
const yaml = require('js-yaml');

const hpp_template = `// Generated code
#pragma once
#include <stdint.h>
const int ITEM_NAME_Level_LEVEL_INDEX_Count = COUNT;
float ITEM_NAME_StartTime_Level_LEVEL_INDEX[];
uint32_t ITEM_NAME_StartLocation_Level_LEVEL_INDEX[];
`;
const cpp_template = `// Generated code
#include <stdint.h>
float ITEM_NAME_StartTime_Level_LEVEL_INDEX[] =
{
START_TIMES
};
uint32_t ITEM_NAME_StartLocation_Level_LEVEL_INDEX[] =
{
START_LOCATIONS
};
`;

const WriteWaveCode = ( level, item_name, item_wave ) => {
  const start_times_text     = item_wave.map( line => line[0].toFixed(2).padStart(8," ") + "f" ).join(",\n");
  const start_locations_text = item_wave.map( line => {
    const location_text  = line[1];
    let location_value = 0;
    for (let i=0;i<20;i++) {
      if (location_text.charAt(i) == "*") {
        location_value = location_value | (1 << i);
      }
    }
    const location_hex = Number(location_value).toString(16);
    const location_hex_pad = "0x" + ("00000000".substr(0, 8 - location_hex.length) + location_hex);
    return "  " + location_hex_pad;
  }).join(",\n");

  let hpp_text = hpp_template;
  let cpp_text = cpp_template;
   
  cpp_text = cpp_text.replace(/ITEM_NAME/g,item_name);
  cpp_text = cpp_text.replace(/LEVEL_INDEX/g,level);
  cpp_text = cpp_text.replace(/START_TIMES/g,start_times_text);
  cpp_text = cpp_text.replace(/START_LOCATIONS/g,start_locations_text);

  hpp_text = hpp_text.replace(/ITEM_NAME/g,item_name);
  hpp_text = hpp_text.replace(/LEVEL_INDEX/g,level);
  hpp_text = hpp_text.replace(/COUNT/g,item_wave.length);

  fs.writeFileSync( "../../generated_src/" + item_name + "_Wave_Level_" + level + ".h", hpp_text );
  fs.writeFileSync( "../../generated_src/" + item_name + "_Wave_Level_" + level + ".cpp", cpp_text );
}

const LoadWave = ( level, filename_in ) => {
  const level_waves = yaml.load( fs.readFileSync( filename_in, 'utf8' ) );
  const item_names  = Object.keys( level_waves );
  item_names.forEach( item_name => {
    const item_wave = level_waves[ item_name ];
    WriteWaveCode( level, item_name, item_wave );
  });
}

LoadWave(1,"waves_1.txt");
LoadWave(2,"waves_2.txt");
LoadWave(3,"waves_3.txt");
LoadWave(4,"waves_4.txt");
