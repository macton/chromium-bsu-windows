#include <float.h>
#include <math.h>
#include <stdint.h>
#include "../src/vec2.h"
#include "../src/compatibility.h"

// Read Buffers
#include "../src/Simulation.h"
#include "Formation.hpp"

// Write Buffer
#include "PowerUp_HeroAmmo01_Live.hpp"

int    g_Simulation_PowerUp_HeroAmmo01_Live_Count;
vec2   g_Simulation_PowerUp_HeroAmmo01_Live_Pos[kSimulation_PowerUp_HeroAmmo01_Live_MaxCount];
float  g_Simulation_PowerUp_HeroAmmo01_Live_Age[kSimulation_PowerUp_HeroAmmo01_Live_MaxCount];
float  g_Simulation_PowerUp_HeroAmmo01_NextSpawnTime;

void
Simulation_PowerUp_HeroAmmo01_Live_Reset()
{
  g_Simulation_PowerUp_HeroAmmo01_Live_Count    = 0;
  g_Simulation_PowerUp_HeroAmmo01_NextSpawnTime = 0.0f;
}

void
Simulation_PowerUp_HeroAmmo01_Live_CreateNew_Level_0()
{
  // Read Buffers
  const float     time                    = g_Simulation_Time;
  const float     time_step               = 49.00f;
  const vec2      base_vel                = kSimulation_PowerUp_HeroAmmo01_Live_BaseVelocity;
  const int       live_max_count          = kSimulation_PowerUp_HeroAmmo01_Live_MaxCount;
  const vec2      simulation_rect         = kSimulation_PlayArea;
  const float     simulation_rect_left    = -simulation_rect.x;
  const float     simulation_rect_hwidth  = simulation_rect.x;
  const float     simulation_rect_width   = 2.0f * simulation_rect.x;
  const float     simulation_rect_top     = simulation_rect.y;
  const float     simulation_rect_bottom  = -simulation_rect.y;
  const float     simulation_time_step    = g_Simulation_TimeStep;
  const uint32_t* spawn_formation         = g_SpawnFormation;

  // Write Buffer
  float           next_spawn_time         = g_Simulation_PowerUp_HeroAmmo01_NextSpawnTime;
  int             live_count              = g_Simulation_PowerUp_HeroAmmo01_Live_Count;
  vec2*           live_pos                = g_Simulation_PowerUp_HeroAmmo01_Live_Pos;
  float*          live_age                = g_Simulation_PowerUp_HeroAmmo01_Live_Age;

  if ( time < next_spawn_time )
  {
    return;
  }
  
  
  
  // Expression
  {
    float time_start     = 3.50f;
    float time_stop      = 300.00f;
    int   in_time_window = (time >= time_start) && (time <= time_stop);
    if ( in_time_window )
    {
      const float width    = -0.80f;
      const float repeat   = 0.50f;
      const float length   = 296.50f;
      const float entry    = width * sinf((repeat * time * M_PI * 2.0)/length);
      const float entry_x  = entry * simulation_rect_hwidth;
      const float entry_y  = simulation_rect_top;
      const int   live_ndx = live_count % live_max_count;

      live_pos[live_ndx] = vec2( entry_x, entry_y );
      live_age[live_ndx] = FLT_MIN;
      live_count++;
    }
  }
  
  
  

  g_Simulation_PowerUp_HeroAmmo01_Live_Count    = live_count;
  g_Simulation_PowerUp_HeroAmmo01_NextSpawnTime = time + time_step;
}



void
Simulation_PowerUp_HeroAmmo01_Live_CreateNew()
{
  // Read Buffers
  const int    level = g_Simulation_Level;

  switch (level)
  {
    
    case 0: Simulation_PowerUp_HeroAmmo01_Live_CreateNew_Level_0(); break;
    
  }
}

void
Simulation_PowerUp_HeroAmmo01_Live_UpdateExisting()
{
  // Read Buffers
  const vec2   simulation_rect         = kSimulation_PlayArea;
  const float  simulation_rect_left    = -simulation_rect.x;
  const float  simulation_rect_width   = 2.0f * simulation_rect.x;
  const float  simulation_rect_top     = simulation_rect.y;
  const float  simulation_rect_bottom  = -simulation_rect.y;
  const float  simulation_time_step    = g_Simulation_TimeStep;
  const vec2   base_vel                = kSimulation_PowerUp_HeroAmmo01_Live_BaseVelocity;
  const int    live_count              = g_Simulation_PowerUp_HeroAmmo01_Live_Count;
  const int    live_max_count          = kSimulation_PowerUp_HeroAmmo01_Live_MaxCount;
  const int    update_count            = min(live_count, live_max_count);

  // Write Buffer
  vec2*        live_pos                = g_Simulation_PowerUp_HeroAmmo01_Live_Pos;
  float*       live_age                = g_Simulation_PowerUp_HeroAmmo01_Live_Age;

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
Simulation_PowerUp_HeroAmmo01_Live_Update()
{
  Simulation_PowerUp_HeroAmmo01_Live_CreateNew();
  Simulation_PowerUp_HeroAmmo01_Live_UpdateExisting();
}
