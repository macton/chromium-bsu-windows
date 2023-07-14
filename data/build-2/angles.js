const GridWidth = 32;
const GridHeight = 32;

const AngleTable = ( x0, y0 ) => {
  const table = new Array(GridHeight);
  for (y1=0;y1<GridHeight;y1++) {
    const row = new Array(GridWidth);
    for (x1=0;x1<GridWidth;x1++) {
      const dx = x1-x0;
      const dy = y1-y0;
      const len = Math.sqrt((dx*dx)+(dy*dy));
      if ( len == 0 ) { 
      } else
        row[x1] = 255;
      {
        const vx = dx/len;
        const vy = dy/len;
        const angle_rad  = Math.atan2(vy,vx) + Math.PI;
        const angle_unit = angle_rad / (Math.PI * 2.0);
        const angle_u8   = (angle_unit*255)|0;
        row[x1] = angle_u8;
      }
    }
    table[y1] = row;
  }
  return table;
}

const AngleTableToString = (table) => {
  let str = "";
  for (y1=0;y1<GridHeight;y1++) {
    for (x1=0;x1<GridWidth;x1++) {
      str += (table[y1][x1] >>> 0).toString(16).padStart(2,'0');
    }
    str += '\n';
  }
  return str;
}

const table_0_0 = AngleTable(0,0);
const table_1_0 = AngleTable(1,0);
const table_2_0 = AngleTable(2,0);

console.log('0,0');
console.log(AngleTableToString(table_0_0));

console.log('1,0');
{
  const table_next = table_1_0;
  const table_prev = table_0_0;
  let str = "";
  for (y1=0;y1<GridHeight;y1++) {
    for (x1=0;x1<GridWidth;x1++) {
      let cell = (table_next[y1][x1] >>> 0).toString(16).padStart(2,'0');
      if (x1 > 0) {
        const cell_prev = (table_prev[y1][x1-1] >>> 0).toString(16).padStart(2,'0');
        if (cell == cell_prev) {
          cell = "--";
        }
      }
      str += cell;
    }
    str += '\n';
  }
  console.log(str);
}


