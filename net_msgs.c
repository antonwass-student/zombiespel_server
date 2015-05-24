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
        client[i].status = false;
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
    char msg[512];

    int index=1, i;
    msg[0]=2;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    Converter_Int32ToBytes(msg, &index, angle);

    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId != objId)
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

void SendBullet(GameObject bullet)
{
    char data[512];
    int index = 1;
    data[0] = NET_OBJECT_BULLET;

    Converter_Int32ToBytes(data, &index, bullet.obj_id);
    Converter_Int32ToBytes(data, &index, bullet.rect.x);
    Converter_Int32ToBytes(data, &index, bullet.rect.y);
    Converter_Int32ToBytes(data, &index, (int)bullet.bulletInfo.angle);
    Converter_Int32ToBytes(data, &index, bullet.bulletInfo.velocity);

    data[index++] = bullet.bulletInfo.type;

    printf("Sending bullet to clients with angle = '%d'\n", bullet.bulletInfo.angle);

    for(int j = 0; j < N_CLIENTS; j++)
    {
        if(client[j].status)
        {
            printf("Bullet - sent\n");
            SDLNet_TCP_Send(client[j].socket, data, 512);
            printf("post bullet\n");
        }
    }
}

void SendPlayerStats()
{
    int x = 2750, y = 5350, damage, health, speed;


    for(int j = 0; j < N_CLIENTS; j++)
    {
        if(client[j].status)
        {
            switch(client[j].pClass)
            {
                case CLASS_SCOUT:
                    damage = 10;
                    health = 50;
                    speed = 8;
                    break;
                case CLASS_SOLDIER:
                    damage = 25;
                    health = 80;
                    speed = 5;
                    break;
                case CLASS_TANK:
                    damage = 20;
                    health = 100;
                    speed = 4;
                    break;
            }

            char data[512];
            int index = 1;

            data[0] = NET_PLAYER_STATS;

            Converter_Int32ToBytes(data, &index, x);
            Converter_Int32ToBytes(data, &index, y);
            Converter_Int32ToBytes(data, &index, damage);
            Converter_Int32ToBytes(data, &index, health);
            Converter_Int32ToBytes(data, &index, speed);

            SDLNet_TCP_Send(client[j].socket, data, 512);
            printf("Stats was sent to '%s'\n", client[j].name);

            x+=200;
        }
    }
}

void SendSyncObjects(Scene* scene){
    int i;
    printf("Sending SyncObjects.\n");
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
            if(client[j].status && scene->objects[i].obj_id != client[j].playerId)
            {
                SDLNet_TCP_Send(client[j].socket, msg, 512);
                printf("object id %d was sent to id:'%d' name'%s'\n",scene->objects[i].obj_id, j,client[j].name);
                printf("X = '%d' , y = '%d'\n", scene->objects[i].rect.x, scene->objects[i].rect.y);
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

void SendPlayerId(int PlayerId){
    unsigned char msg[512];
    int index=1;
    msg[0]=7;
    printf("Player id = '%d'\n",PlayerId);
    Converter_Int32ToBytes(msg, &index, PlayerId);
    printf("debug1\n");
    for(int i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId == PlayerId)
            SDLNet_TCP_Send(client[i].socket, msg, 512);
    }


}

void SendLobbyPlayer(char* playerName, char pClass, int playerId)
{
    unsigned char data[512];
    int index = 1;
    int length = strlen(playerName);

    data[0] = NET_LOBBY_PLAYER;

    Converter_Int32ToBytes(data, &index, length);

    for(int i = 0; i < length; i++)
    {
        data[index + i] = playerName[i];
        printf("Packing down name at index %d\n", index + i);
    }
    index += length;

    data[index++] = pClass;

    printf("Sending player with name %s to clients\n", playerName);

    for(int i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status == true)
        {
            printf("Comparing player ids...\n");
            SDLNet_TCP_Send(client[i].socket, data, 512);
            /*
            if(client[i].playerId != playerId)
            {
                SDLNet_TCP_Send(client[i].socket, data, 512);
                printf("Sent player to %s\n", client[i].name);
            }*/
        }
    }

}

void RecvPlayerShoot(char data[], Scene* scene)
{
    int index = 1;
    int shooterId, x, y, angle, damage, speed;
    bulletType_T bType;

    shooterId = Converter_BytesToInt32(data, &index);
    x = Converter_BytesToInt32(data, &index);
    y = Converter_BytesToInt32(data, &index);
    angle = Converter_BytesToInt32(data, &index);
    damage = Converter_BytesToInt32(data, &index);
    speed = Converter_BytesToInt32(data, &index);

    printf("Received a shoot with x = '%d' y = '%d' speed = '%d'\n",x, y, speed);

    GameObject bullet = CreateBullet(scene->nextId++, x, y, damage, angle, speed, BULLET_PLAYER);
    AddObject(scene, bullet, false); //net är false eftersom att bullet är ett speciellt objekt.
    SendBullet(bullet);

}

void RecvPlayerPos(char data[], Scene* scene){
    int index = 1;
    int playerId, x, y, angle;


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
            SendObjectPos(playerId, x, y, angle);
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
