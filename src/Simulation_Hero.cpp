#include "vec2.h"

// Read Buffers
#include "Input.h"

// Write Buffer
#include "Simulation_Hero.h"

vec2 g_Simulation_Hero_Pos;

void Simulation_Hero_Reset()
{
    g_Simulation_Hero_Pos = kSimulation_Hero_Reset_Pos;
}

void Simulation_Hero_Update()
{
    vec2 vel = vec2(0.0f, 0.0f);
    vec2 pos = g_Simulation_Hero_Pos;

    if (g_InputPadDown & (1<<kInputPadLeft))
    {
        vel += vec2(-0.4f, 0.0f);
    }
    if (g_InputPadDown & (1<<kInputPadRight))
    {
        vel += vec2(0.4f, 0.0f);
    }
    if (g_InputPadDown & (1<<kInputPadUp))
    {
        vel += vec2(0.0f, 0.4f);
    }
    if (g_InputPadDown & (1<<kInputPadDown))
    {
        vel += vec2(0.0f, -0.4f);
    }

    g_Simulation_Hero_Pos = clamp(pos+vel, vec2(-kSimulation_Hero_PlayArea.x, -kSimulation_Hero_PlayArea.y), kSimulation_Hero_PlayArea);
}
