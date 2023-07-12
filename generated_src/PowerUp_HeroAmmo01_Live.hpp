#pragma once
#include "../src/vec2.h"

const int  kSimulation_PowerUp_HeroAmmo01_Live_MaxCount     = 1;
const vec2 kSimulation_PowerUp_HeroAmmo01_Live_BaseSize     = vec2(0.60f, 0.60f);
const vec2 kSimulation_PowerUp_HeroAmmo01_Live_BaseVelocity = vec2(0.00f, -0.05f);

extern int   g_Simulation_PowerUp_HeroAmmo01_Live_Count;
extern vec2  g_Simulation_PowerUp_HeroAmmo01_Live_Pos[kSimulation_PowerUp_HeroAmmo01_Live_MaxCount];
extern float g_Simulation_PowerUp_HeroAmmo01_Live_Age[kSimulation_PowerUp_HeroAmmo01_Live_MaxCount];

void Simulation_PowerUp_HeroAmmo01_Live_Reset();
void Simulation_PowerUp_HeroAmmo01_Live_Update();
