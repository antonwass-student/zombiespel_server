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

int AddObject(Scene* scene, GameObject object)
{
    pthread_mutex_lock(&scene->object_mutex);
    scene->objects[scene->objCount] = object;
    scene->objCount++;
    pthread_mutex_unlock(&scene->object_mutex);
    return 0;
}

int RemoveObject(Scene* scene, int id)
{
    int index= -1, i;
    pthread_mutex_lock(&scene->object_mutex);
    for(i=0;i<scene->objCount;i++)
    {
        if(scene->objects[i].obj_id==id)
        {
            index=i;
            break;
        }

    }

    if(index==-1){
        pthread_mutex_unlock(&scene->object_mutex);
        return -1;
    }


    for(i=index;i<scene->objCount-1;i++)
    {
        scene->objects[i]=scene->objects[i+1];
    }
    scene->objCount--;
    pthread_mutex_unlock(&scene->object_mutex);
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

GameObject CreatePlayer(int x, int y, int id)
{
    GameObject object;
    object.x=x;
    object.y=y;
    object.obj_id=id;
    object.type=OBJECT_PLAYER;
    return object;
}
