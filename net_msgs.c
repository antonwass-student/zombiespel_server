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

void connectionInit()
{
    for(int i = 0; i < N_CLIENTS; i++)
    {
        client[i].ready = false;
    }
}

int Converter_BytesToInt32(unsigned char data[], int* index){ // Gör om en byte-array till en int.

    int value = 0;
    value += (((int)data[*index]) << 24);
    value += (((int)data[*index + 1]) << 16);
    value += (((int)data[*index + 2]) << 8);
    value += (((int)data[*index + 3]));

    *index += 4;

    return value;
}

int Converter_Int32ToBytes(unsigned char data[], int* size, int value) //Gör om en int till en byte array.
{
    data[(*size + 3)] = (value);
    data[*size + 2] = ((value) >> 8);
    data[*size + 1] = ((value) >> 16);
    data[*size] = ((value) >> 24);

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
    unsigned char msg[512];
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

void SendNewObject(int objId, int x, int y, objectType_t type)
{
    unsigned char msg[512];
    int index=1, i;

    msg[0]=5;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    msg[index++]=type;

    for(i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status)
            SDLNet_TCP_Send(client[i].socket, msg, 512);
    }

}

void SendGameStart()
{
    char data[512];
    data[0] = NET_GAME_START;

    for(int j = 0; j < N_CLIENTS; j++)
    {
        if(client[j].status)
        {
            printf("Game start - sent\n");
            SDLNet_TCP_Send(client[j].socket, data, 512);
        }
    }

}

void SendSyncObjects(Scene* scene){
    int i;
    printf("sending sync objects.\n");
    for(i = 0; i < scene->objCount; i++)
    {
        char msg[512];
        int index=1;
        msg[0]=5;
        Converter_Int32ToBytes(msg, &index, scene->objects[i].obj_id);
        Converter_Int32ToBytes(msg, &index, scene->objects[i].rect.x);
        Converter_Int32ToBytes(msg, &index, scene->objects[i].rect.y);
        msg[index++]=scene->objects[i].type;

        for(int j = 0; j < N_CLIENTS; j++)
        {
            if(client[j].status)
            {
                SDLNet_TCP_Send(client[j].socket, msg, 512);
                printf("object id %d was sent to '%d'\n",scene->objects[i].obj_id, client[j].name);
            }
        }
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

void SendPlayerId(int PlayerId, int clientID){
    unsigned char msg[512];
    int index=1;
    msg[0]=7;
    printf("Player id = '%d'\n",PlayerId);
    Converter_Int32ToBytes(msg, &index, PlayerId);
    printf("debug1\n");
    SDLNet_TCP_Send(client[clientID].socket, msg, 512);

}

void SendLobbyPlayer(char* playerName, char pClass)
{
    unsigned char data[512];
    int index = 1;
    int length = strlen(playerName);

    data[0] = 11;

    Converter_Int32ToBytes(data, &index, length);

    for(int i = 0; i < length; i++)
    {
        data[index + i] = playerName[i];
    }
    index += length;

    data[index] = pClass;

    for(int i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status == true)
        {
            if(strcmp(client[i].name, playerName))
            {
                SDLNet_TCP_Send(client[i].socket, data, 512);
            }
        }
    }

}

void RecvPlayerPos(unsigned char data[], Scene* scene){
    int index = 1;
    int playerId, x, y, angle;
    printf("received player pos\n");

    playerId = Converter_BytesToInt32(data, &index);
    x = Converter_BytesToInt32(data, &index);
    y = Converter_BytesToInt32(data, &index);
    angle = (float)Converter_BytesToInt32(data, &index);

    for(int i = 0; i < 128; i++)
    {
        if(scene->objects[i].obj_id == playerId)
        {
            scene->objects[i].rect.x = x;
            scene->objects[i].rect.y = y;
            printf("player pos x=%d, y=%d, angle=%d\n",x, y, angle);
            break;
        }
    }
}

void RecvPlayerReady(unsigned char data[])
{
    int index = 1;
    int playerId = Converter_BytesToInt32(data, &index);

    for(int i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status && client[i].playerId == playerId)
        {
            printf("Player %s is ready\n", client[i].name);
            client[i].ready = true;
        }
    }
}
