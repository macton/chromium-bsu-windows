#pragma once
#include "vec2.h"

const int  kSimulation_EnemyAircraft_Straight_Live_MaxCount = 32;
const vec2 kSimulation_EnemyAircraft_Straight_Live_Size = vec2(0.75f, 1.02f);
const vec2 kSimulation_EnemyAircraft_Straight_Live_BaseVel = vec2(0.0f, -0.066f);

extern int   g_Simulation_EnemyAircraft_Straight_Live_Count;
extern vec2  g_Simulation_EnemyAircraft_Straight_Live_Pos[kSimulation_EnemyAircraft_Straight_Live_MaxCount];
extern vec2  g_Simulation_EnemyAircraft_Straight_Live_Vel[kSimulation_EnemyAircraft_Straight_Live_MaxCount];
extern float g_Simulation_EnemyAircraft_Straight_Live_Age[kSimulation_EnemyAircraft_Straight_Live_MaxCount];

void Simulation_EnemyAircraft_Straight_Live_Reset();
void Simulation_EnemyAircraft_Straight_Live_Update();