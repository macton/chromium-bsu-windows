#include "compatibility.h"
#include <GL/gl.h>
#include "vec2.h"

// Read Buffers
#include "Atlas.h"
#include "Simulation.h"
#include "Simulation_EnemyAmmo00.h"

void Draw_EnemyAmmo00()
{
    int    ammo_live_max_count = kSimulation_EnemyAmmo00_Live_MaxCount;
    int    ammo_live_count = g_Simulation_EnemyAmmo00_Live_Count;
    int    ammo_update_count = ammo_live_count % ammo_live_max_count;
    vec2 size = kSimulation_EnemyAmmo00_Live_Size;

    for (int ammo_index = 0; ammo_index < ammo_update_count; ammo_index++)
    {
        float age = g_Simulation_EnemyAmmo00_Live_Age[ammo_index];
        if (age > 0.0f)
        {
            vec2  pos = g_Simulation_EnemyAmmo00_Live_Pos[ammo_index];

            glColor4f(1.0, 1.0, 1.0, 1.0);
            AtlasDrawSprite(kEnemyAmmo00, pos.x, pos.y, size.x, size.y);
        }
    }
}