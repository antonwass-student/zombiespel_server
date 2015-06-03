#include "server_structs.h"
#include "server_collision.h"
#include "spel_objects.h"
#include "spel_ai.h"

int Update(Scene* scene,bool netUpdate)//Updates all the objects in the scene
{
    for(int i = 0; i < scene->objCount; i++)
    {
        int x = 0, y = 0;
        if(scene->objects[i].type == OBJECT_BULLET) //Bullet update
        {
            if(scene->objects[i].bulletInfo.timetolive > 0)
                scene->objects[i].bulletInfo.timetolive--;
            else if(scene->objects[i].bulletInfo.timetolive == 0)//Bullet time expired
            {
                RemoveObject(scene, scene->objects[i].obj_id);
                continue;
            }
            
            y -= sin((scene->objects[i].bulletInfo.angle + 90) * M_PI / 180.0f) * scene->objects[i].bulletInfo.velocity;
            x -= cos((scene->objects[i].bulletInfo.angle + 90) * M_PI / 180.0f) * scene->objects[i].bulletInfo.velocity;
            MoveObject(&scene->objects[i], scene, x, y, i);
        }
        else if(scene->objects[i].type == OBJECT_NPC || scene->objects[i].type == OBJECT_ZOMBIE_SPITTER ) //NPC update
        {
            Zombie_UseBrain(scene, &scene->objects[i],i, netUpdate);
        }
        else if(scene->objects[i].type == OBJECT_PLAYER)
        {
            MoveObject(&scene->objects[i], scene, 0, 0, i);
        }
    }
    return EXIT_SUCCESS;
}
