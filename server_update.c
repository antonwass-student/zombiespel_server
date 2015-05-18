#include "server_structs.h"
#include "server_collision.h"

int Update(Scene* scene)
{
    for(int i = 0; i < scene->objCount; i++)
    {
        int x, y;

        if(scene->objects[i].type == OBJECT_BULLET) //Bullet update
        {
            if(scene->objects[i].bulletInfo.timetolive > 0)
                scene->objects[i].bulletInfo.timetolive--;
            else if(scene->objects[i].bulletInfo.timetolive == 0)
            {
                RemoveObject(scene, scene->objects[i].obj_id);
                continue;
            }

            y -= sin((scene->objects[i].bulletInfo.angle + 90) * M_PI / 180.0f) * scene->objects[i].bulletInfo.velocity;
            x -= cos((scene->objects[i].bulletInfo.angle + 90) * M_PI / 180.0f) * scene->objects[i].bulletInfo.velocity;
            MoveObject(&scene->objects[i], scene, x, y, i);
        }
        else if(scene->objects[i].type == OBJECT_NPC) //NPC update
        {
            Zombie_UseBrain(scene, &scene->objects[i],i);
        }
    }
}
