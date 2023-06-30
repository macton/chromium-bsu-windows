#pragma once
const int     kSimulation_EnemyAmmo00_Live_MaxCount = 32;
const vec2    kSimulation_EnemyAmmo00_Live_Size = vec2(0.25f, 0.55f);
const vec2    kSimulation_EnemyAmmo00_Live_BaseVel = vec2(0.0f, -0.2f);
const vec2    kSimulation_EnemyAmmo00_EnemyAircraft_Straight_Turret_Pos_Offset = vec2(0.0f, -0.9f);

extern int    g_Simulation_EnemyAmmo00_Live_Count;
extern vec2   g_Simulation_EnemyAmmo00_Live_Pos[kSimulation_EnemyAmmo00_Live_MaxCount];
extern float  g_Simulation_EnemyAmmo00_Live_Age[kSimulation_EnemyAmmo00_Live_MaxCount];

void Simulation_EnemyAmmo00_Live_Reset();
void Simulation_EnemyAmmo00_Live_Update();
