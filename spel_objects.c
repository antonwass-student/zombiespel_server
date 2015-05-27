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
    {
        if(object.type == OBJECT_ITEM)
        {
            SendNewObject(object.obj_id, object.rect.x, object.rect.y, object.itemInfo.type, object.name);
            //printf("debug2\n");
        }
        else
        {
            SendNewObject(object.obj_id, object.rect.x, object.rect.y, object.type, object.name);
        }

    }


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
            //printf("Object removed with id %d\n", id);
            SendRemoveObject(id);
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

GameObject CreateMachineGun(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_ITEM;
    object.solid = false;
    strcpy(object.name, "Machine gun\0");

    object.itemInfo.type = ITEM_WEAPON_1;
    object.itemInfo.amount = 1;

    printf("Machine gun created\n");

    return object;
}

GameObject CreateShotgun(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_ITEM;
    object.solid = false;
    strcpy(object.name, "Shotgun\0");

    object.itemInfo.type = ITEM_WEAPON_2;
    object.itemInfo.amount = 1;

    printf("Shotgun created\n");

    return object;
}

GameObject CreateRevolver(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_ITEM;
    object.solid = false;
    strcpy(object.name, "Revolver\0");

    object.itemInfo.type = ITEM_WEAPON_3;
    object.itemInfo.amount = 1;

    printf("Revolver created\n");

    return object;
}

GameObject CreateArmor(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_ITEM;
    object.solid = false;
    strcpy(object.name, "Armor\0");

    object.itemInfo.type = ITEM_ARMOR;
    object.itemInfo.amount = 20;

    printf("Armor created\n");

    return object;
}

GameObject CreateAmmo(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_ITEM;
    object.solid = false;
    strcpy(object.name, "Ammo\0");

    object.itemInfo.type = ITEM_AMMO;
    object.itemInfo.amount = 20;

    printf("Ammo created\n");

    return object;
}

GameObject CreateMedkit(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_ITEM;
    object.solid = false;
    strcpy(object.name, "Medkit\0");

    object.itemInfo.type = ITEM_MEDKIT;
    object.itemInfo.amount = 40;

    printf("Medkit created\n");

    return object;
}

GameObject CreateZombie(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 128;
    object.rect.h = 128;
    object.obj_id=id;
    object.type=OBJECT_NPC;
    object.solid = false;
    strcpy(object.name, "Zombie");

    object.ai.health = 100;
    object.ai.attackRange = 100;
    object.ai.detectRange = 400;
    object.ai.speed = 5;
    object.ai.ai = AI_ZOMBIE;
    object.ai.atkCd = 120;
    object.ai.atkTimer = 30;
    object.ai.damage = 30;
    object.ai.target = NULL;
    return object;
}

GameObject CreateZombieSpitter(int x, int y, int id)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 128;
    object.rect.h = 128;
    object.obj_id=id;
    object.type=OBJECT_ZOMBIE_SPITTER;
    object.solid = false;
    strcpy(object.name, "ZombieSpitter");

    object.ai.health = 100;
    object.ai.attackRange = 300;
    object.ai.detectRange = 400;
    object.ai.speed = 5;
    object.ai.ai = AI_SPITTER;
    object.ai.atkCd = 30;
    object.ai.atkTimer = 30;
    object.ai.damage = 30;
    object.ai.target = NULL;

    object.ai.fireRate = 60;
    object.ai.fireCount = 60;
    return object;
}


GameObject CreatePlayer(int x, int y, int id, char* name)
{
    GameObject object;
    object.rect.x=x;
    object.rect.y=y;
    object.rect.w = 128;
    object.rect.h = 128;
    object.obj_id = id;

    strcpy(object.name,name);
    object.playerInfo.alive = true;
    object.playerInfo.health = 100;
    object.playerInfo.ammoTotal = 0;

    object.type = OBJECT_PLAYER;
    return object;
}

GameObject CreateBullet(int id, int x, int y, int damage, int direction, int velocity, bulletType_T bType)
{
    GameObject object;
    object.rect.x = x;
    object.rect.y = y;
    object.rect.w = 20;
    object.rect.h = 20;
    object.obj_id = id;
    object.type = OBJECT_BULLET;

    object.bulletInfo.angle = direction;
    object.bulletInfo.damage = damage;
    object.bulletInfo.velocity = velocity;
    object.bulletInfo.type = bType;
    object.bulletInfo.timetolive = 120;

    //printf("new bullet x = %d, y = %d\n", x, y);


    return object;

}
