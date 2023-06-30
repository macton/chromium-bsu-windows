#include <math.h>
#include <float.h>
#include "vec2.h"

// Read Buffers
#include "Simulation.h"
#include "Simulation_EnemyAircraft_Straight.h"

// Write Buffer
#include "Simulation_EnemyAmmo00.h"

int    g_Simulation_EnemyAmmo00_Live_Count;
vec2   g_Simulation_EnemyAmmo00_Live_Pos[kSimulation_EnemyAmmo00_Live_MaxCount];
float  g_Simulation_EnemyAmmo00_Live_Age[kSimulation_EnemyAmmo00_Live_MaxCount];
float  g_Simulation_EnemyAmmo00_Live_EnemyAircraft_Straight_Live_LastAge[kSimulation_EnemyAircraft_Straight_Live_MaxCount];

void Simulation_EnemyAmmo00_Live_Reset()
{
    g_Simulation_EnemyAmmo00_Live_Count = 0;
}

float EnemyAircraft_Straight_EnemyAmmo00_NextFireAge(float age)
{
    // Rate of fire = 1/sec
    return floorf(age) + 1.0f;
}

void Simulation_EnemyAmmo00_Live_CreateFrom_EnemyAircraft_Straight()
{
    float  next_update_time     = g_Simulation_Time;
    vec2   turret_pos_offset    = kSimulation_EnemyAmmo00_EnemyAircraft_Straight_Turret_Pos_Offset;

    int    ammo_live_max_count  = kSimulation_EnemyAmmo00_Live_MaxCount;
    int    ammo_live_count      = g_Simulation_EnemyAmmo00_Live_Count;
    int    ammo_update_count    = ammo_live_count % ammo_live_max_count;

    int    enemy_live_count     = g_Simulation_EnemyAircraft_Straight_Live_Count;
    int    enemy_live_max_count = kSimulation_EnemyAircraft_Straight_Live_MaxCount;
    int    enemy_update_count   = enemy_live_count % enemy_live_max_count;

    vec2*  enemy_live_pos       = g_Simulation_EnemyAircraft_Straight_Live_Pos;
    float* enemy_live_age       = g_Simulation_EnemyAircraft_Straight_Live_Age;

    for (int enemy_index = 0; enemy_index < enemy_update_count; enemy_index++)
    {
        float enemy_age = enemy_live_age[enemy_index];
        if (enemy_age > 0.0f)
        {
            float enemy_last_age        = g_Simulation_EnemyAmmo00_Live_EnemyAircraft_Straight_Live_LastAge[enemy_index];
            float enemy_next_fire_age   = EnemyAircraft_Straight_EnemyAmmo00_NextFireAge(enemy_last_age);
            if ((enemy_last_age < enemy_next_fire_age) && (enemy_age > enemy_next_fire_age))
            {
                vec2  enemy_pos = enemy_live_pos[enemy_index];
                vec2  ammo_pos  = enemy_pos + turret_pos_offset;
                float ammo_age  = FLT_MIN;
                int   ammo_index = ammo_live_count % ammo_live_max_count;

                g_Simulation_EnemyAmmo00_Live_Pos[ammo_index] = ammo_pos;
                g_Simulation_EnemyAmmo00_Live_Age[ammo_index] = ammo_age;
                ammo_live_count++;
            }
            g_Simulation_EnemyAmmo00_Live_EnemyAircraft_Straight_Live_LastAge[enemy_index] = enemy_age;
        }
    }
    g_Simulation_EnemyAmmo00_Live_Count = ammo_live_count;
}

void Simulation_EnemyAmmo00_Live_Update()
{
    Simulation_EnemyAmmo00_Live_CreateFrom_EnemyAircraft_Straight();

    int    ammo_live_max_count = kSimulation_EnemyAmmo00_Live_MaxCount;
    int    ammo_live_count = g_Simulation_EnemyAmmo00_Live_Count;
    int    ammo_update_count = ammo_live_count % ammo_live_max_count;

    vec2            ammo_vel = kSimulation_EnemyAmmo00_Live_BaseVel;
    vec2            simulation_rect = g_Simulation_Rect;
    float           simulation_rect_left = -simulation_rect.x;
    float           simulation_rect_width = 2.0f * simulation_rect.x;
    float           simulation_rect_top = simulation_rect.y;
    float           simulation_rect_bottom = -simulation_rect.y;
    float           simulation_time_step = g_Simulation_TimeStep;

    for (int ammo_index = 0; ammo_index < ammo_update_count; ammo_index++)
    {
        float ammo_age = g_Simulation_EnemyAmmo00_Live_Age[ammo_index];
        if (ammo_age > 0.0f)
        {
            vec2 ammo_pos = g_Simulation_EnemyAmmo00_Live_Pos[ammo_index];

            ammo_pos = ammo_pos + ammo_vel;
            ammo_age = (ammo_pos.y < simulation_rect_bottom) ? 0.0f : (ammo_age + simulation_time_step);

            g_Simulation_EnemyAmmo00_Live_Pos[ammo_index] = ammo_pos;
            g_Simulation_EnemyAmmo00_Live_Age[ammo_index] = ammo_age;
        }
    }
}