

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

    if(zombie->ai.target == NULL)
    {
        //printf("Searching for a target.\n");
        zombie->ai.target = FindPlayer(scene, zombie, zombie->ai.detectRange);
    }
    if(zombie->ai.target == NULL)
            return;

    dx = zombie->rect.x - (zombie->ai.target->x + (zombie->ai.target->w /2));
    dy = zombie->rect.y - (zombie->ai.target->y + (zombie->ai.target->h /2));
    zombie->rotation = 270 + (atan2(dy,dx)*180/M_PI);

    dx = 0;
    dy = 0;

    dy -= sin((zombie->rotation + 90) * M_PI / 180.0f) * zombie->ai.speed; //Objektets y hastighet
    dx -= cos((zombie->rotation + 90) * M_PI / 180.0f) * zombie->ai.speed; //objektets x hastighet

    if(GetDistance(*zombie->ai.target,zombie->rect) > zombie->ai.attackRange)
    {
        MoveObject(zombie, scene, dx,dy, index);

        //printf("Sent object pos to server\n");
    }
    else if(GetDistance(*zombie->ai.target,zombie->rect) < zombie->ai.attackRange)
    {
        if(zombie->ai.ai == AI_SPITTER)
            Zombie_Shoot(zombie, scene);
        else if(zombie->ai.ai == AI_ZOMBIE)
            Zombie_Attack(zombie, scene);
    }
    SendObjectPos(zombie->obj_id, zombie->rect.x, zombie->rect.y, (int)zombie->rotation);
}

int Zombie_Attack(GameObject* zombie, Scene* scene)
{
    //printf("zombie attacked\n");
    zombie->ai.atkTimer--;
    for(int i = 0; i < scene->objCount; i++)
    {
        if(scene->objects[i].type == OBJECT_PLAYER)
        {
            int distance = GetDistance(zombie->rect, scene->objects[i].rect);

            if(distance < zombie->ai.attackRange && zombie->ai.atkTimer <= 0)
            {
                scene->objects[i].playerInfo.health -= zombie->ai.damage;
                zombie->ai.atkTimer = zombie->ai.atkCd;
                printf("Player was damaged by Zombie.\n");
                printf("New health is: %d\n", scene->objects[i].playerInfo.health);
                SendPlayerHealth(scene->objects[i].obj_id, scene->objects[i].playerInfo.health);
                // TODO:
                // SEND A MESSAGE TO CLIENT WHO TOOK DMG
            }
            break;
        }
    }
}

int Zombie_Shoot(GameObject* zombie, Scene* scene)
{
    if(zombie->ai.fireCount > 0){
        zombie->ai.fireCount--;
    }

    if(zombie->ai.fireCount == 0){
        printf("Zombie fired.\n");

        GameObject obj;
        obj = CreateBullet(scene->nextId++, zombie->rect.x, zombie->rect.y, zombie->ai.damage, zombie->rotation, zombie->ai.bulletSpeed, BULLET_ZOMBIE);
        AddObject(scene, obj, false);
        SendBullet(obj);
        //newObject = createObject(scene, OBJECT_ZBULLET, "Spit", zombie->rect.x + (zombie->rect.w/2),zombie->rect.y + (zombie->rect.h/2), 20, 20, TXT_ZBULLET, false);
        //SetBulletStats(&scene->objects[newObject], zombie->ai.bulletSpeed, zombie->rotation, zombie->ai.damage);
        zombie->ai.fireCount = zombie->ai.fireRate;
    }

    return EXIT_SUCCESS;

}

SDL_Rect* FindPlayer(Scene* scene, GameObject* zombie, int range)
{
    //printf("Initiated FindPlayer()\n");
    for(int i = 0; i < scene->objCount ; i++)
    {
        int distance = 10;

        if(scene->objects[i].type == OBJECT_PLAYER && scene->objects[i].playerInfo.alive)
        {
            distance = (int)(sqrt(pow(zombie->rect.x - scene->objects[i].rect.x, 2) + pow(zombie->rect.y - scene->objects[i].rect.y, 2)));


            if(distance < range)
            {
                zombie->ai.targetIsAlive = &scene->objects[i].playerInfo.alive;
                printf("Zombie has chosen a target! Beware\n");
                return &scene->objects[i].rect;
            }
        }
    }

    return NULL;
}
