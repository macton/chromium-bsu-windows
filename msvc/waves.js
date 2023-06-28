const fs = require('fs');


const writeWavesFile = (filename_in, filename_out) => {
  const level_file = fs.readFileSync(filename_in);
  const level_text = level_file.toString();
  const level_lines = level_text.split("\n");
  const type_lines = {};
  level_lines.forEach( line => {
    	const fields = line.split(/\s+/);
  	const type_name = fields[0].trim();
  	if (!type_lines.hasOwnProperty(type_name)) {
  		type_lines[type_name] = [];
  	}
  	type_lines[type_name].push( [ parseFloat(fields[1]), fields[2] ] );
  });
  const type_names = Object.keys( type_lines );
  
  const strChar = (str, index, replacement) => {
      return str.substring(0, index) + replacement + str.substring(index + replacement.length);
  }
  
  let waves_file_text = "";
  type_names.forEach( type_name => {
    if (type_name.length == 0) {
  	  return;
    }
    const lines = type_lines[type_name].sort( (a,b) => a[0]-b[0] );
    waves_file_text += type_name + ":" + "\n";
    // console.log(type_name + ":");
  
    let last_line_text = "--------------------";
    let last_line_time = "0";
    let last_line_ready = false;
    lines.forEach( line => {
  	const line_time  = line[0].toFixed(4).padStart(10," ");
  	const line_index = line[1]|0;
          if ( line_time != last_line_time ) {
            if ( last_line_ready) {
              // console.log( last_line_time + " " + last_line_text );
              waves_file_text += last_line_time + " " + last_line_text + "\n";
  	  } 
            last_line_text = "--------------------";
  	}
  
  	const line_text  = strChar( last_line_text, line_index, "*" );
          last_line_time  = line_time;
  	last_line_text  = line_text;   
          last_line_ready = true; 
    });
    if ( last_line_ready) {
      // console.log( last_line_time + " " + last_line_text );
      waves_file_text += last_line_time + " " + last_line_text + "\n";
    }
  });
  fs.writeFileSync( filename_out, waves_file_text );
}

writeWavesFile("Level1","waves_1.txt");
writeWavesFile("Level2","waves_2.txt");
writeWavesFile("Level3","waves_3.txt");
writeWavesFile("Level4","waves_4.txt");
