#include "compatibility.h"
#include <GL/gl.h>
#include "vec2.h"

// Read Buffers
#include "Atlas.h"
#include "Simulation.h"
#include "Simulation_EnemyAircraft_Straight.h"

void Draw_EnemyAircraft_Straight()
{
    int    enemy_live_count = g_Simulation_EnemyAircraft_Straight_Live_Count;
    int    enemy_live_max_count = kSimulation_EnemyAircraft_Straight_Live_MaxCount;
    int    enemy_update_count = enemy_live_count % enemy_live_max_count;

    vec2 size = kSimulation_EnemyAircraft_Straight_Live_Size;
    for (int enemy_index = 0; enemy_index < enemy_update_count; enemy_index++)
    {
        float age = g_Simulation_EnemyAircraft_Straight_Live_Age[enemy_index];
        if (age > 0.0f)
        {
            vec2  pos = g_Simulation_EnemyAircraft_Straight_Live_Pos[enemy_index];

            glColor4f(1.0, 1.0, 1.0, 1.0);
            AtlasDrawSprite(kEnemy00, pos.x, pos.y, size.x, size.y);
        }
    }
}