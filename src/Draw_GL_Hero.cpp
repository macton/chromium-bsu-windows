#include "compatibility.h"
#include <GL/gl.h>
#include "vec2.h"

// Read Buffers
#include "Atlas.h"
#include "Simulation.h"
#include "Simulation_Hero.h"

void Draw_Hero()
{
    vec2 pos = g_Simulation_Hero_Pos;
    vec2 size = kSimulation_Hero_Size;

    glColor4f(1.0, 1.0, 1.0, 1.0);
    AtlasDrawSprite(kHero, pos.x, pos.y, size.x, size.y);
}