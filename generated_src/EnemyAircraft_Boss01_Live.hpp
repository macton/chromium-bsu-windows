#pragma once
#include "../src/vec2.h"

const int  kSimulation_EnemyAircraft_Boss01_Live_MaxCount     = 8;
const vec2 kSimulation_EnemyAircraft_Boss01_Live_BaseSize     = vec2(2.6000f, 2.3000f);
const vec2 kSimulation_EnemyAircraft_Boss01_Live_BaseVelocity = vec2(0.0000f, -0.0200f);

extern int   g_Simulation_EnemyAircraft_Boss01_Live_Count;
extern vec2  g_Simulation_EnemyAircraft_Boss01_Live_Pos[kSimulation_EnemyAircraft_Boss01_Live_MaxCount];
extern vec2  g_Simulation_EnemyAircraft_Boss01_Live_Vel[kSimulation_EnemyAircraft_Boss01_Live_MaxCount];
extern float g_Simulation_EnemyAircraft_Boss01_Live_Age[kSimulation_EnemyAircraft_Boss01_Live_MaxCount];

void Simulation_EnemyAircraft_Boss01_Live_Reset();
void Simulation_EnemyAircraft_Boss01_Live_Update();
