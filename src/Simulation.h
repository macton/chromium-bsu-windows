#pragma once
#include "vec2.h"

// #todo move globals into packed simulation buffer
// #todo velocities in units/sec not units/frame

extern int   g_Simulation_Level;
extern int   g_Simulation_Frame;
extern float g_Simulation_Time;
extern float g_Simulation_TimeStep;
extern vec2  g_Simulation_Rect;

void Simulation_Reset();
void Simulation_Update();