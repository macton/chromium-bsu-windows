#pragma once
#include "../src/vec2.h"

const int  kSimulation_PowerUp_Shields_Live_MaxCount     = 1;
const vec2 kSimulation_PowerUp_Shields_Live_BaseSize     = vec2(0.6000f, 0.6000f);
const vec2 kSimulation_PowerUp_Shields_Live_BaseVelocity = vec2(0.0000f, -0.0500f);

extern int   g_Simulation_PowerUp_Shields_Live_Count;
extern vec2  g_Simulation_PowerUp_Shields_Live_Pos[kSimulation_PowerUp_Shields_Live_MaxCount];
extern vec2  g_Simulation_PowerUp_Shields_Live_Vel[kSimulation_PowerUp_Shields_Live_MaxCount];
extern float g_Simulation_PowerUp_Shields_Live_Age[kSimulation_PowerUp_Shields_Live_MaxCount];

void Simulation_PowerUp_Shields_Live_Reset();
void Simulation_PowerUp_Shields_Live_Update();
