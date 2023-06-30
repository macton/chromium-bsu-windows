#include <float.h>
#include "vec2.h"

// Read Buffers
#include "../generated_src/EnemyAircraft_Straight_Wave_Level_1.h"
#include "../generated_src/EnemyAircraft_Straight_Wave_Level_2.h"
#include "../generated_src/EnemyAircraft_Straight_Wave_Level_3.h"
#include "../generated_src/EnemyAircraft_Straight_Wave_Level_4.h"
#include "Simulation.h"

// Write Buffer
#include "Simulation_EnemyAircraft_Straight.h"

int    g_Simulation_EnemyAircraft_Straight_Live_Count;
vec2   g_Simulation_EnemyAircraft_Straight_Live_Pos[kSimulation_EnemyAircraft_Straight_Live_MaxCount];
vec2   g_Simulation_EnemyAircraft_Straight_Live_Vel[kSimulation_EnemyAircraft_Straight_Live_MaxCount]; // #todo vel = f(index)
float  g_Simulation_EnemyAircraft_Straight_Live_Age[kSimulation_EnemyAircraft_Straight_Live_MaxCount];
int    s_Simulation_EnemyAircraft_Straight_Live_LastWaveIndex;

void
Simulation_EnemyAircraft_Straight_Live_Reset()
{
  g_Simulation_EnemyAircraft_Straight_Live_Count         = 0;
  s_Simulation_EnemyAircraft_Straight_Live_LastWaveIndex = 0;
}

void
Simulation_EnemyAircraft_Straight_Live_CreateFrom_Wave()
{
    const int       level_wave_count[] = { EnemyAircraft_Straight_Level_1_Count, EnemyAircraft_Straight_Level_2_Count, EnemyAircraft_Straight_Level_3_Count, EnemyAircraft_Straight_Level_4_Count };
    const float* level_wave_start_time[] = { EnemyAircraft_Straight_StartTime_Level_1, EnemyAircraft_Straight_StartTime_Level_2, EnemyAircraft_Straight_StartTime_Level_3, EnemyAircraft_Straight_StartTime_Level_4 };
    const uint32_t* level_wave_start_location[] = { EnemyAircraft_Straight_StartLocation_Level_1, EnemyAircraft_Straight_StartLocation_Level_2, EnemyAircraft_Straight_StartLocation_Level_3, EnemyAircraft_Straight_StartLocation_Level_4 };
    const vec2      base_vel = kSimulation_EnemyAircraft_Straight_Live_BaseVel;

    int    enemy_live_count = g_Simulation_EnemyAircraft_Straight_Live_Count;
    int    enemy_live_max_count = kSimulation_EnemyAircraft_Straight_Live_MaxCount;
    int    enemy_update_count = enemy_live_count % enemy_live_max_count;

    int             wave_ndx = s_Simulation_EnemyAircraft_Straight_Live_LastWaveIndex;
    int             level = g_Simulation_Level;
    float           time = g_Simulation_Time;
    int             wave_count = level_wave_count[level];
    const float* wave_start_time = level_wave_start_time[level];
    const uint32_t* wave_start_location = level_wave_start_location[level];

    vec2            simulation_rect = g_Simulation_Rect;
    float           simulation_rect_left = -simulation_rect.x;
    float           simulation_rect_width = 2.0f * simulation_rect.x;
    float           simulation_rect_top = simulation_rect.y;
    float           simulation_rect_bottom = -simulation_rect.y;
    float           simulation_time_step = g_Simulation_TimeStep;

    while (wave_ndx < wave_count)
    {
        float    start_time = wave_start_time[wave_ndx];
        uint32_t start_location = wave_start_location[wave_ndx];
        if (start_time > time)
        {
            break;
        }
        for (int i = 0; i < 20; i++)
        {
            if (start_location & (1 << i))
            {
                float start_x = simulation_rect_left + (((float)i * simulation_rect_width) / 20.0f) + 0.5f;
                vec2  start_pos = vec2(start_x, simulation_rect_top);
                vec2  start_vel = base_vel;
                float start_age = FLT_MIN;
                int   live_index = enemy_live_count % enemy_live_max_count;

                g_Simulation_EnemyAircraft_Straight_Live_Pos[live_index] = start_pos;
                g_Simulation_EnemyAircraft_Straight_Live_Vel[live_index] = start_vel;
                g_Simulation_EnemyAircraft_Straight_Live_Age[live_index] = start_age;

                enemy_live_count++;
            }
        }
        wave_ndx++;
    }

    s_Simulation_EnemyAircraft_Straight_Live_LastWaveIndex = wave_ndx;
    g_Simulation_EnemyAircraft_Straight_Live_Count = enemy_live_count;
}


void
Simulation_EnemyAircraft_Straight_Live_Update()
{
    Simulation_EnemyAircraft_Straight_Live_CreateFrom_Wave();

    int    enemy_live_count = g_Simulation_EnemyAircraft_Straight_Live_Count;
    int    enemy_live_max_count = kSimulation_EnemyAircraft_Straight_Live_MaxCount;
    int    enemy_update_count = enemy_live_count % enemy_live_max_count;

    vec2            simulation_rect = g_Simulation_Rect;
    float           simulation_rect_left = -simulation_rect.x;
    float           simulation_rect_width = 2.0f * simulation_rect.x;
    float           simulation_rect_top = simulation_rect.y;
    float           simulation_rect_bottom = -simulation_rect.y;
    float           simulation_time_step = g_Simulation_TimeStep;

    for (int live_ndx = 0; live_ndx < enemy_update_count; live_ndx++)
    {
        float age = g_Simulation_EnemyAircraft_Straight_Live_Age[live_ndx];
        if (age > 0.0f)
        {
            vec2  pos = g_Simulation_EnemyAircraft_Straight_Live_Pos[live_ndx];
            vec2  vel = g_Simulation_EnemyAircraft_Straight_Live_Vel[live_ndx];

            pos = pos + vel;
            age = (pos.y < simulation_rect_bottom)?0.0f:(age + simulation_time_step);

            g_Simulation_EnemyAircraft_Straight_Live_Pos[live_ndx] = pos;
            g_Simulation_EnemyAircraft_Straight_Live_Age[live_ndx] = age;
        }
    }
}
