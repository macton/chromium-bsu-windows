#pragma once
#include "../src/vec2.h"

const int  kSimulation_EnemyAmmo00_Live_MaxCount     = 128;
const vec2 kSimulation_EnemyAmmo00_Live_BaseSize     = vec2(0.25f, 0.25f);
const vec2 kSimulation_EnemyAmmo00_Live_BaseVelocity = vec2(0.00f, -0.20f);

extern int   g_Simulation_EnemyAmmo00_Live_Count;
extern vec2  g_Simulation_EnemyAmmo00_Live_Pos[kSimulation_EnemyAmmo00_Live_MaxCount];
extern float g_Simulation_EnemyAmmo00_Live_Age[kSimulation_EnemyAmmo00_Live_MaxCount];

void Simulation_EnemyAmmo00_Live_Reset();
void Simulation_EnemyAmmo00_Live_Update();
