#include "../src/compatibility.h"
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include "../src/vec2.h"

// Read Buffers
#include "../src/Simulation.h"

#include "EnemyAircraft_Straight_Live.hpp"

#include "EnemyAircraft_Omni_Live.hpp"


// Write Buffer
#include "EnemyAmmo00_Live.hpp"

int    g_Simulation_EnemyAmmo00_Live_Count;
vec2   g_Simulation_EnemyAmmo00_Live_Pos[kSimulation_EnemyAmmo00_Live_MaxCount];
float  g_Simulation_EnemyAmmo00_Live_Age[kSimulation_EnemyAmmo00_Live_MaxCount];

// Lookup spawn by quantized age.
// #todo pattern of different sizes?

void Simulation_EnemyAmmo00_Live_Reset()
{
  g_Simulation_EnemyAmmo00_Live_Count = 0;
}


void Simulation_EnemyAmmo00_Live_CreateNew_0()
{
  uint32_t spawn_pattern        = 0xffffffff;
  float    spawn_time_step      = 1.00;
  float    spawn_inv_time_step  = 1.0f / spawn_time_step;
  vec2     spawn_pos_offset     = vec2( 0.00f, -0.90f );

  int      live_max_count       = kSimulation_EnemyAmmo00_Live_MaxCount;
  int      live_count           = g_Simulation_EnemyAmmo00_Live_Count;
  int      update_count         = min(live_count, live_max_count);

  int      spawn_live_count     = g_Simulation_EnemyAircraft_Straight_Live_Count;
  int      spawn_live_max_count = kSimulation_EnemyAircraft_Straight_Live_MaxCount;
  int      spawn_update_count   = min(spawn_live_count, spawn_live_max_count);

  vec2*    spawn_live_pos       = g_Simulation_EnemyAircraft_Straight_Live_Pos;
  float*   spawn_live_age       = g_Simulation_EnemyAircraft_Straight_Live_Age;

  for (int spawn_index = 0; spawn_index < spawn_update_count; spawn_index++)
  {
    float spawn_age = spawn_live_age[spawn_index];
    if (spawn_age > 0.0f)
    {
      int spawn_pattern_index = ((int)floorf(spawn_age * spawn_inv_time_step))&0x1f;
      if ( spawn_pattern & (1<<spawn_pattern_index) )
      {
        // only spawn at raising edge
        float prev_spawn_age = spawn_age - g_Simulation_TimeStep;
        float edge_spawn_age = ((float)spawn_pattern_index) * spawn_time_step;
        if ( prev_spawn_age < edge_spawn_age )
        {
          vec2  spawn_pos = spawn_live_pos[spawn_index];
          vec2  pos       = spawn_pos + spawn_pos_offset;
          float age       = FLT_MIN;
          int   index     = live_count % live_max_count;

          g_Simulation_EnemyAmmo00_Live_Pos[index] = pos;
          g_Simulation_EnemyAmmo00_Live_Age[index] = age;
          live_count++;
        }
      }
    }
  }
  g_Simulation_EnemyAmmo00_Live_Count = live_count;
}

void Simulation_EnemyAmmo00_Live_CreateNew_1()
{
  uint32_t spawn_pattern        = 0x00ff00ff;
  float    spawn_time_step      = 0.10;
  float    spawn_inv_time_step  = 1.0f / spawn_time_step;
  vec2     spawn_pos_offset     = vec2( 0.00f, -0.90f );

  int      live_max_count       = kSimulation_EnemyAmmo00_Live_MaxCount;
  int      live_count           = g_Simulation_EnemyAmmo00_Live_Count;
  int      update_count         = min(live_count, live_max_count);

  int      spawn_live_count     = g_Simulation_EnemyAircraft_Omni_Live_Count;
  int      spawn_live_max_count = kSimulation_EnemyAircraft_Omni_Live_MaxCount;
  int      spawn_update_count   = min(spawn_live_count, spawn_live_max_count);

  vec2*    spawn_live_pos       = g_Simulation_EnemyAircraft_Omni_Live_Pos;
  float*   spawn_live_age       = g_Simulation_EnemyAircraft_Omni_Live_Age;

  for (int spawn_index = 0; spawn_index < spawn_update_count; spawn_index++)
  {
    float spawn_age = spawn_live_age[spawn_index];
    if (spawn_age > 0.0f)
    {
      int spawn_pattern_index = ((int)floorf(spawn_age * spawn_inv_time_step))&0x1f;
      if ( spawn_pattern & (1<<spawn_pattern_index) )
      {
        // only spawn at raising edge
        float prev_spawn_age = spawn_age - g_Simulation_TimeStep;
        float edge_spawn_age = ((float)spawn_pattern_index) * spawn_time_step;
        if ( prev_spawn_age < edge_spawn_age )
        {
          vec2  spawn_pos = spawn_live_pos[spawn_index];
          vec2  pos       = spawn_pos + spawn_pos_offset;
          float age       = FLT_MIN;
          int   index     = live_count % live_max_count;

          g_Simulation_EnemyAmmo00_Live_Pos[index] = pos;
          g_Simulation_EnemyAmmo00_Live_Age[index] = age;
          live_count++;
        }
      }
    }
  }
  g_Simulation_EnemyAmmo00_Live_Count = live_count;
}



void Simulation_EnemyAmmo00_Live_UpdateExisting()
{
  int    ammo_live_max_count    = kSimulation_EnemyAmmo00_Live_MaxCount;
  int    ammo_live_count        = g_Simulation_EnemyAmmo00_Live_Count;
  int    ammo_update_count      = min(ammo_live_count, ammo_live_max_count);
  vec2   ammo_vel               = kSimulation_EnemyAmmo00_Live_BaseVelocity;

  vec2   simulation_rect        = kSimulation_PlayArea;
  float  simulation_rect_left   = -simulation_rect.x;
  float  simulation_rect_width  = 2.0f * simulation_rect.x;
  float  simulation_rect_top    = simulation_rect.y;
  float  simulation_rect_bottom = -simulation_rect.y;
  float  simulation_time_step   = g_Simulation_TimeStep;

  for (int ammo_index = 0; ammo_index < ammo_update_count; ammo_index++)
  {
    float ammo_age = g_Simulation_EnemyAmmo00_Live_Age[ammo_index];
    if (ammo_age > 0.0f)
    {
      vec2 ammo_pos = g_Simulation_EnemyAmmo00_Live_Pos[ammo_index];

      ammo_pos = ammo_pos + ammo_vel;
      if (ammo_pos.y < simulation_rect_bottom)
      {
          printf("Kill Straight age:%f\n", ammo_age);

      }

      ammo_age = (ammo_pos.y < simulation_rect_bottom) ? 0.0f : (ammo_age + simulation_time_step);

      g_Simulation_EnemyAmmo00_Live_Pos[ammo_index] = ammo_pos;
      g_Simulation_EnemyAmmo00_Live_Age[ammo_index] = ammo_age;
    }
 }
}

void Simulation_EnemyAmmo00_Live_Update()
{

  Simulation_EnemyAmmo00_Live_CreateNew_0();

  Simulation_EnemyAmmo00_Live_CreateNew_1();


  Simulation_EnemyAmmo00_Live_UpdateExisting();
}
