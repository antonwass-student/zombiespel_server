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

pthread_mutex_t object_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t objID_mutex=PTHREAD_MUTEX_INITIALIZER;
int objID=100;
int objCount=0;

int AddObject(GameObject objects[], GameObject object, int *size)
{
    pthread_mutex_lock(&object_mutex);
    objects[*size] = object;
    (*size)++;
    pthread_mutex_unlock(&object_mutex);
    return 0;
}

int RemoveObject(GameObject objects[], int id, int *size)
{
    int index= -1, i;
    pthread_mutex_lock(&object_mutex);
    for(i=0;i<(*size);i++)
    {
        if(objects[i].obj_id==id)
        {
            index=i;
            break;
        }
        
    }
    
    if(index==-1){
        pthread_mutex_unlock(&object_mutex);
        return -1;
    }
    
    
    for(i=index;i<(*size)-1;i++)
    {
        objects[i]=objects[i+1];
    }
    (*size)--;
    pthread_mutex_unlock(&object_mutex);
    return 0;
}

GameObject CreateZombie(int x, int y)
{
    GameObject object;
    object.x=x;
    object.y=y;
    pthread_mutex_lock(&objID_mutex);
    object.obj_id=objID;
    objID++;
    pthread_mutex_unlock(&objID_mutex);
    object.type=OBJECT_ZOMBIE_NORMAL;
    return object;
}

GameObject CreatePlayer(int x, int y)
{
    GameObject object;
    object.x=x;
    object.y=y;
    pthread_mutex_lock(&objID_mutex);
    object.obj_id=objID;
    objID++;
    pthread_mutex_unlock(&objID_mutex);
    object.type=OBJECT_PLAYER;
    return object;
}