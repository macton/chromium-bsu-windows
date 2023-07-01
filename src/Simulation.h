#pragma once
#include "vec2.h"

// #todo move globals into packed simulation buffer
// #todo velocities in units/sec not units/frame

const  vec2 kSimulation_PlayArea = vec2(10.0f, 9.0f);


extern int   g_Simulation_Level;
extern int   g_Simulation_Frame;
extern float g_Simulation_Time;
extern float g_Simulation_TimeStep;

void Simulation_Reset();
void Simulation_Update();