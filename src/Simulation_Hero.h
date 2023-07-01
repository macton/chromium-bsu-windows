#pragma once
#include "vec2.h"

const vec2 kSimulation_Hero_PlayArea = vec2(10.0f, 7.5f);
const vec2 kSimulation_Hero_Reset_Pos = vec2(0.0f, -3.0f);
const vec2 kSimulation_Hero_Size = vec2(0.7f, 0.85f);

extern vec2 g_Simulation_Hero_Pos;
extern vec2 g_Simulation_Hero_Vel;

void Simulation_Hero_Reset();
void Simulation_Hero_Update();
