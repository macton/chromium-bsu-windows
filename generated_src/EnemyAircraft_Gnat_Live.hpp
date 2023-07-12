#pragma once
#include "../src/vec2.h"

const int  kSimulation_EnemyAircraft_Gnat_Live_MaxCount     = 8;
const vec2 kSimulation_EnemyAircraft_Gnat_Live_BaseSize     = vec2(0.45f, 0.45f);
const vec2 kSimulation_EnemyAircraft_Gnat_Live_BaseVelocity = vec2(0.00f, -0.02f);

extern int   g_Simulation_EnemyAircraft_Gnat_Live_Count;
extern vec2  g_Simulation_EnemyAircraft_Gnat_Live_Pos[kSimulation_EnemyAircraft_Gnat_Live_MaxCount];
extern float g_Simulation_EnemyAircraft_Gnat_Live_Age[kSimulation_EnemyAircraft_Gnat_Live_MaxCount];

void Simulation_EnemyAircraft_Gnat_Live_Reset();
void Simulation_EnemyAircraft_Gnat_Live_Update();
