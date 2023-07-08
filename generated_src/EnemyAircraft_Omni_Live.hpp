#pragma once
#include "../src/vec2.h"

const int  kSimulation_EnemyAircraft_Omni_Live_MaxCount     = 8;
const vec2 kSimulation_EnemyAircraft_Omni_Live_BaseSize     = vec2(0.7000f, 0.7000f);
const vec2 kSimulation_EnemyAircraft_Omni_Live_BaseVelocity = vec2(0.0000f, -0.0900f);

extern int   g_Simulation_EnemyAircraft_Omni_Live_Count;
extern vec2  g_Simulation_EnemyAircraft_Omni_Live_Pos[kSimulation_EnemyAircraft_Omni_Live_MaxCount];
extern vec2  g_Simulation_EnemyAircraft_Omni_Live_Vel[kSimulation_EnemyAircraft_Omni_Live_MaxCount];
extern float g_Simulation_EnemyAircraft_Omni_Live_Age[kSimulation_EnemyAircraft_Omni_Live_MaxCount];

void Simulation_EnemyAircraft_Omni_Live_Reset();
void Simulation_EnemyAircraft_Omni_Live_Update();
