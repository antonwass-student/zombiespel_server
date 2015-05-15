//
//  net_msgs.c
//  SDL_net
//
//  Created by cristian araya on 06/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//
#ifdef __APPLE__
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

#include "server_structs.h"
#include "net_msgs.h"
#include "spel_objects.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


#define N_CLIENTS 2
msg_stack recvPool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

void poolInit(){
    recvPool.size=0;
}

int Converter_BytesToInt32(char data[], int* index){ // Gör om en byte-array till en int.

    int value = 0;
    value += (((int)data[*index]) << 24);
    value += (((int)data[*index + 1]) << 16);
    value += (((int)data[*index + 2]) << 8);
    value += (((int)data[*index + 3]));

    *index += 4;

    return value;
}

int Converter_Int32ToBytes(char data[], int* size, int value) //Gör om en int till en byte array.
{
    data[(*size + 3)] = ( char)(value);
    data[*size + 2] = ( char)((value) >> 8);
    data[*size + 1] = ( char)((value) >> 16);
    data[*size] = ( char)((value) >> 24);

    *size += 4;

    return 0;
}

int AddToPool(char* msg) // Funktion fˆr att l‰gga till meddelanden i stacks ( pools).
{

    pthread_mutex_lock(&pool_mutex);
    memcpy(recvPool.queue[recvPool.size], msg, 512);
    recvPool.size++;
    pthread_mutex_unlock(&pool_mutex);
    return 0;
}

void SendObjectPos(int objId, int x, int y, int angle)
{
     char msg[512];
    int index=1, i;
    msg[0]=2;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    Converter_Int32ToBytes(msg, &index, angle);

    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true)
            SDLNet_TCP_Send(client[i].socket, msg, 512);
    }

}

void SendNewObject(int objId, int x, int y, objectType_t type, int clientID)
{
     char msg[512];
    int index=1, i;
    msg[0]=5;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    msg[index++]=type;
    SDLNet_TCP_Send(client[clientID].socket, msg, 512);
}

void SendSyncObjects(int clientID, Scene* scene){
    int i;
    for (i=0; i<scene->objCount; i++)
    {
        //SendNewObject(scene->objects[i].obj_id, scene->objects[i].x, scene->objects[i].y, scene->objects[i].type, int clientID);
    }

}

void SendRemoveObject(int objId)
{
     char msg[512];
    int index=1, i;
    msg[0]=6;
    Converter_Int32ToBytes(msg, &index, objId);

    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true)
            SDLNet_TCP_Send(client[i].socket, msg, 512);
    }

}

void SendPlayerId(int PlayerId, int i){
     char msg[512];
    int index=1;
    msg[0]=7;
    printf("playerid %d\n",PlayerId);
    Converter_Int32ToBytes(msg, &index, PlayerId);
    SDLNet_TCP_Send(client[i].socket, msg, 512);
    index=1;
    int test;
    test = Converter_BytesToInt32(msg, &index);
    printf("%d",test);
}

void RecvPlayerPos(char data[], Scene* scene){
    int index = 1;
    int playerId, x, y, angle;
    printf("received player pos\n");

    playerId = Converter_BytesToInt32(data, &index);
    x = Converter_BytesToInt32(data, &index);
    y = Converter_BytesToInt32(data, &index);
    angle = (float)Converter_BytesToInt32(data, &index);

    for(int i = 0; i < 128; i++)
    {
        if(scene->objects[i].obj_id + 1500 == playerId)
        {
            scene->objects[i].x = x;
            scene->objects[i].y = y;
            printf("player pos x=%d, y=%d, angle=%d\n",x, y, angle);
            break;
        }
    }
}
