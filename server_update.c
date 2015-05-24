#include "server_structs.h"
#include "server_collision.h"

int Update(Scene* scene)
{
    for(int i = 0; i < scene->objCount; i++)
    {
        int x = 0, y = 0;

        if(scene->objects[i].type == OBJECT_BULLET) //Bullet update
        {
            if(scene->objects[i].bulletInfo.timetolive > 0)
                scene->objects[i].bulletInfo.timetolive--;
            else if(scene->objects[i].bulletInfo.timetolive == 0)
            {
                printf("Bullet time expired. removing id %d\n", scene->objects[i].obj_id);
                RemoveObject(scene, scene->objects[i].obj_id);
                continue;
            }

            y -= sin((scene->objects[i].bulletInfo.angle + 90) * M_PI / 180.0f) * scene->objects[i].bulletInfo.velocity;
            x -= cos((scene->objects[i].bulletInfo.angle + 90) * M_PI / 180.0f) * scene->objects[i].bulletInfo.velocity;
            MoveObject(&scene->objects[i], scene, x, y, i);
            printf("new bullet x = %d, y = %d\n", scene->objects[i].rect.x, scene->objects[i].rect.y);
        }
        else if(scene->objects[i].type == OBJECT_NPC) //NPC update
        {
            Zombie_UseBrain(scene, &scene->objects[i],i);
        }
    }
}
