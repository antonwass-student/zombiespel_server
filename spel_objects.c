//
//  spel_objects.c
//  SDL_net
//
//  Created by cristian araya on 08/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#endif

#include <stdio.h>
#include <pthread.h>
#include "server_structs.h"


int AddObject(Scene* scene, GameObject object, bool net)
{
    SDL_LockMutex(scene->obj_mutex);
    scene->objects[scene->objCount] = object;
    scene->objCount++;
    SDL_UnlockMutex(scene->obj_mutex);

    //skicka till klienterna
    if(net)
        SendNewObject(object.obj_id, object.rect.x, object.rect.y, object.type);

    return 0;
}

int RemoveObject(Scene* scene, int id)
{
    int index= -1, i;
    SDL_LockMutex(scene->obj_mutex);
    for(i=0;i<scene->objCount;i++)
    {
        if(scene->objects[i].obj_id==id)
        {
            printf("Object removed\n");
            index=i;
            break;
        }

    }

    if(index==-1){
        SDL_UnlockMutex(scene->obj_mutex);
        return -1;
    }


    for(i=index;i<scene->objCount-1;i++)
    {
        scene->objects[i]=scene->objects[i+1];
    }
    scene->objCount--;
    SDL_UnlockMutex(scene->obj_mutex);
    return 0;
}

GameObject CreateZombie(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.obj_id=id;
    object.type=OBJECT_NPC;

    object.ai.attackRange = 50;
    object.ai.detectRange = 300;
    object.ai.speed = 5;
    object.ai.ai = AI_ZOMBIE;
    object.ai.atkCd = 30;
    object.ai.atkTimer = 30;
    object.ai.damage = 30;
    return object;
}

GameObject CreatePlayer(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.obj_id = id;

    object.type=OBJECT_PLAYER;
    return object;
}

GameObject CreateBullet(int id, int x, int y, int damage, int direction, int velocity, bulletType_T bType)
{
    GameObject object;
    object.rect.x = x;
    object.rect.y = y;
    object.obj_id = id;
    object.type = OBJECT_BULLET;

    object.bulletInfo.angle = direction;
    object.bulletInfo.damage = damage;
    object.bulletInfo.velocity = velocity;
    object.bulletInfo.type = bType;
    object.bulletInfo.timetolive = 120;


    return object;

}
