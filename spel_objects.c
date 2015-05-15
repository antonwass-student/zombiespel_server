//
//  spel_objects.c
//  SDL_net
//
//  Created by cristian araya on 08/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//

#include <stdio.h>
#include <pthread.h>
#include "server_structs.h"
#include <SDL2/SDL_net.h>

int AddObject(Scene* scene, GameObject object)
{
    SDL_LockMutex(scene->obj_mutex);
    scene->objects[scene->objCount] = object;
    scene->objCount++;
    SDL_UnlockMutex(scene->obj_mutex);

    //skicka till klienterna
    SendNewObject(object.obj_id, object.x, object.y, object.type);

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
    object.x=x;
    object.y=y;
    object.obj_id=id;
    object.type=OBJECT_ZOMBIE_NORMAL;
    return object;
}

GameObject CreatePlayer(int x, int y, int* id)
{
    GameObject object;
    object.x=x;
    object.y=y;
    object.obj_id = id;
    object.type=OBJECT_PLAYER;
    return object;
}
