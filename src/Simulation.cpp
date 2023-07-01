#include "vec2.h"

int   g_Simulation_Level;
int   g_Simulation_Frame;
float g_Simulation_Time;
float g_Simulation_TimeStep;

void Simulation_Reset()
{
    g_Simulation_Frame = 0;
}

void Simulation_Update()
{
    g_Simulation_Frame++;
    g_Simulation_TimeStep = (1.0f / 60.0f); // #todo fixup with real time later.
    g_Simulation_Time = g_Simulation_Frame * g_Simulation_TimeStep;
}