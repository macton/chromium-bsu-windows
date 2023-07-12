#include <float.h>
#include <math.h>
#include <stdint.h>
#include "../src/vec2.h"
#include "../src/compatibility.h"

// Read Buffers
#include "../src/Simulation.h"
#include "Formation.hpp"

// Write Buffer
#include "EnemyAircraft_Boss01_Live.hpp"

int    g_Simulation_EnemyAircraft_Boss01_Live_Count;
vec2   g_Simulation_EnemyAircraft_Boss01_Live_Pos[kSimulation_EnemyAircraft_Boss01_Live_MaxCount];
float  g_Simulation_EnemyAircraft_Boss01_Live_Age[kSimulation_EnemyAircraft_Boss01_Live_MaxCount];
float  g_Simulation_EnemyAircraft_Boss01_NextSpawnTime;

void
Simulation_EnemyAircraft_Boss01_Live_Reset()
{
  g_Simulation_EnemyAircraft_Boss01_Live_Count    = 0;
  g_Simulation_EnemyAircraft_Boss01_NextSpawnTime = 0.0f;
}




void
Simulation_EnemyAircraft_Boss01_Live_CreateNew()
{
  // Read Buffers
  const int    level = g_Simulation_Level;

  switch (level)
  {
    
  }
}

void
Simulation_EnemyAircraft_Boss01_Live_UpdateExisting()
{
  // Read Buffers
  const vec2   simulation_rect         = kSimulation_PlayArea;
  const float  simulation_rect_left    = -simulation_rect.x;
  const float  simulation_rect_width   = 2.0f * simulation_rect.x;
  const float  simulation_rect_top     = simulation_rect.y;
  const float  simulation_rect_bottom  = -simulation_rect.y;
  const float  simulation_time_step    = g_Simulation_TimeStep;
  const vec2   base_vel                = kSimulation_EnemyAircraft_Boss01_Live_BaseVelocity;
  const int    live_count              = g_Simulation_EnemyAircraft_Boss01_Live_Count;
  const int    live_max_count          = kSimulation_EnemyAircraft_Boss01_Live_MaxCount;
  const int    update_count            = min(live_count, live_max_count);

  // Write Buffer
  vec2*        live_pos                = g_Simulation_EnemyAircraft_Boss01_Live_Pos;
  float*       live_age                = g_Simulation_EnemyAircraft_Boss01_Live_Age;

  for (int live_ndx = 0; live_ndx < update_count; live_ndx++)
  {
    float age = live_age[live_ndx];

    if (age > 0.0f)
    {
      vec2 pos = live_pos[live_ndx] + base_vel;
      age      = (pos.y < simulation_rect_bottom)?0.0f:(age + simulation_time_step);

      live_pos[live_ndx] = pos;
      live_age[live_ndx] = age;
    }
  }
}

void
Simulation_EnemyAircraft_Boss01_Live_Update()
{
  Simulation_EnemyAircraft_Boss01_Live_CreateNew();
  Simulation_EnemyAircraft_Boss01_Live_UpdateExisting();
}
