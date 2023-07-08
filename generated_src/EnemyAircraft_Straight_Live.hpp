#pragma once
#include "../src/vec2.h"

const int  kSimulation_EnemyAircraft_Straight_Live_MaxCount     = 8;
const vec2 kSimulation_EnemyAircraft_Straight_Live_BaseSize     = vec2(0.7500f, 1.0200f);
const vec2 kSimulation_EnemyAircraft_Straight_Live_BaseVelocity = vec2(0.0000f, -0.0500f);

extern int   g_Simulation_EnemyAircraft_Straight_Live_Count;
extern vec2  g_Simulation_EnemyAircraft_Straight_Live_Pos[kSimulation_EnemyAircraft_Straight_Live_MaxCount];
extern vec2  g_Simulation_EnemyAircraft_Straight_Live_Vel[kSimulation_EnemyAircraft_Straight_Live_MaxCount];
extern float g_Simulation_EnemyAircraft_Straight_Live_Age[kSimulation_EnemyAircraft_Straight_Live_MaxCount];

void Simulation_EnemyAircraft_Straight_Live_Reset();
void Simulation_EnemyAircraft_Straight_Live_Update();
