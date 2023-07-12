#include "../src/compatibility.h"
#include <GL/gl.h>
#include "../src/vec2.h"

// Read Buffers
#include "Atlas.hpp"
#include "../src/Simulation.h"
#include "EnemyAircraft_Boss01_Live.hpp"

void Draw_EnemyAircraft_Boss01()
{
  int live_count     = g_Simulation_EnemyAircraft_Boss01_Live_Count;
  int live_max_count = kSimulation_EnemyAircraft_Boss01_Live_MaxCount;
  int update_count   = min( live_count, live_max_count );

  vec2 size = kSimulation_EnemyAircraft_Boss01_Live_BaseSize;
  for (int index = 0; index < update_count; index++)
  {
    float age = g_Simulation_EnemyAircraft_Boss01_Live_Age[index];
    if (age > 0.0f)
    {
      vec2  pos = g_Simulation_EnemyAircraft_Boss01_Live_Pos[index];

      glColor4f(1.0, 1.0, 1.0, 1.0);
      AtlasDrawSprite(kEnemy06, pos.x, pos.y, size.x, size.y);
    }
  }
}
