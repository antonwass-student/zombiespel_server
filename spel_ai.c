

#include "server_structs.h"
#include "spel_objects.h"
#include <math.h>
#define M_PI 3.14

SDL_Rect* FindPlayer(Scene* scene, GameObject* zombie, int range);

void Zombie_UseBrain(Scene* scene, GameObject* zombie, int index)
{
    int dx, dy;

    if(zombie->ai.atkTimer > 0)
    {
        zombie->ai.atkTimer--;
    }

    if(zombie->ai.target == NULL || zombie->ai.targetIsAlive)
    {
        zombie->ai.target = FindPlayer(scene, zombie,zombie->ai.detectRange);
    }
    if(zombie->ai.target == NULL)
            return;

    dx = zombie->rect.x - zombie->ai.target->x;
    dy = zombie->rect.y - zombie->ai.target->y;
    zombie->rotation = 270 + (atan2(dy,dx)*180/M_PI);

    dx = 0;
    dy = 0;

    dy -= sin((zombie->rotation + 90) * M_PI / 180.0f) * zombie->ai.speed; //Objektets y hastighet
    dx -= cos((zombie->rotation + 90) * M_PI / 180.0f) * zombie->ai.speed; //objektets x hastighet

    if(GetDistance(*zombie->ai.target,zombie->rect) > zombie->ai.attackRange)
    {
        MoveObject(zombie, scene, dx,dy, index);
        SendObjectPos(zombie->obj_id, zombie->rect.x, zombie->rect.y, (int)zombie->rotation);
        printf("Sent object pos\n");
    }
    if(zombie->ai.ai == AI_SPITTER && GetDistance(*zombie->ai.target,zombie->rect) < zombie->ai.attackRange)
    {
        Zombie_Shoot(zombie, scene);
    }
}

int Zombie_Shoot(GameObject* zombie, Scene* scene)
{
    if(zombie->ai.fireCount > 0){
        zombie->ai.fireCount--;
    }

    if(zombie->ai.fireCount == 0){

        GameObject obj;
        obj = CreateBullet(scene->nextId++, zombie->rect.x, zombie->rect.y, BULLET_ZOMBIE, zombie->ai.damage, zombie->rotation, 10);
        AddObject(scene, obj, true);
        //newObject = createObject(scene, OBJECT_ZBULLET, "Spit", zombie->rect.x + (zombie->rect.w/2),zombie->rect.y + (zombie->rect.h/2), 20, 20, TXT_ZBULLET, false);
        //SetBulletStats(&scene->objects[newObject], zombie->ai.bulletSpeed, zombie->rotation, zombie->ai.damage);
        //zombie->ai.fireCount = zombie->ai.fireRate;
    }

    return EXIT_SUCCESS;

}

SDL_Rect* FindPlayer(Scene* scene, GameObject* zombie, int range)
{
    for(int i = 0; i < scene->objCount ; i++)
    {
        int distance = 10;

        if(scene->objects[i].type == OBJECT_PLAYER && scene->objects[i].playerInfo.alive)
        {
            distance = (int)(sqrt(pow(zombie->rect.x - scene->objects[i].rect.x, 2) + pow(zombie->rect.y - scene->objects[i].rect.y, 2)));

            if(distance < range)
            {
                zombie->ai.targetIsAlive = &scene->objects[i].playerInfo.alive;
                printf("Found target\n");
                return &scene->objects[i].rect;
            }
        }
    }

    return NULL;
}
