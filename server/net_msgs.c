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


msg_stack recvPool;
pthread_mutex_t pool_mutex = PTHREAD_MUTEX_INITIALIZER;

//Initializes netmessage pool
void poolInit(){
    recvPool.size=0;
}

//Initializes the connection array (array with clients)
void connectionInit()
{
    int i;
    for(i = 0; i < N_CLIENTS; i++)
    {
        client[i].status = false;
        client[i].ready = false;
    }
}
//Converts bytes into an int 32
int Converter_BytesToInt32(unsigned char data[], int* index){

    int value = 0;
    value += (((int)data[*index]) << 24);
    value += (((int)data[*index + 1]) << 16);
    value += (((int)data[*index + 2]) << 8);
    value += (((int)data[*index + 3]));

    *index += 4;

    return value;
}

//Converts an int to 4 bytes.
int Converter_Int32ToBytes(unsigned char data[], int* size, int value)
{
    data[(*size + 3)] = (value);
    data[*size + 2] = ((value) >> 8);
    data[*size + 1] = ((value) >> 16);
    data[*size] = ((value) >> 24);

    *size += 4;

    return 0;
}

//Function that adds messages to the netmessage stack/buffer/pool.
int AddToPool(char* msg)
{

    pthread_mutex_lock(&pool_mutex);
    memcpy(recvPool.queue[recvPool.size], msg, 128);
    recvPool.size++;
    pthread_mutex_unlock(&pool_mutex);
    return 0;
}

void SendObjectPos(int objId, int x, int y, int angle)
{
    unsigned char msg[128];

    int index=1, i;
    msg[0]=2;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    Converter_Int32ToBytes(msg, &index, angle);

    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId != objId)
            SDLNet_TCP_Send(client[i].socket, msg, 128);
    }

}

void SendNewObject(int objId, int x, int y, objectType_t type, char* name)
{
    unsigned char msg[128];
    int index=1, i;

    msg[0]=5;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    msg[index++]=type;
    Converter_Int32ToBytes(msg, &index,(int)strlen(name));
    //printf("Sent object%s(%d) to clients\n", name, objId);

    for(i = 0; i < strlen(name); i++)
    {
        msg[index++] = name[i];
    }

    for(i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status)
            SDLNet_TCP_Send(client[i].socket, msg, 128);
    }

}

void SendGameStart()
{
    char data[128];
    data[0] = NET_GAME_START;
    int j;
    for(j = 0; j < N_CLIENTS; j++)
    {
        if(client[j].status)
        {
            printf("Game start - sent\n");
            SDLNet_TCP_Send(client[j].socket, data, 128);
        }
    }
}

void SendBullet(GameObject bullet)
{
    unsigned char data[128];
    int index = 1, j;
    data[0] = NET_OBJECT_BULLET;

    Converter_Int32ToBytes(data, &index, bullet.obj_id);
    Converter_Int32ToBytes(data, &index, bullet.rect.x);
    Converter_Int32ToBytes(data, &index, bullet.rect.y);
    Converter_Int32ToBytes(data, &index, (int)bullet.bulletInfo.angle);
    Converter_Int32ToBytes(data, &index, bullet.bulletInfo.velocity);

    data[index++] = bullet.bulletInfo.type;

    //printf("Sending bullet to clients with angle = '%d'\n", bullet.bulletInfo.angle);

    for(j = 0; j < N_CLIENTS; j++)
    {
        if(client[j].status)
        {

            SDLNet_TCP_Send(client[j].socket, data, 128);
        }
    }
}

void SendPlayerStats(Scene* scene)
{
    int j, x = 2750, y = 5350, damage, health, speed;


    for(j = 0; j < N_CLIENTS; j++)
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
                case CLASS_ENGINEER:
                    damage = 15;
                    health = 90;
                    speed = 6;
                    break;

            }

            for(int i = 0; i < scene->objCount; i++)
            {
                if(client[j].playerId == scene->objects[i].obj_id)
                {
                    scene->objects[i].playerInfo.health = health;
                    scene->objects[i].playerInfo.damage = damage;
                    scene->objects[i].playerInfo.speed = speed;
                    break;
                }
            }


            unsigned char data[128];

            int index = 1;

            data[0] = NET_PLAYER_STATS;

            Converter_Int32ToBytes(data, &index, x);
            Converter_Int32ToBytes(data, &index, y);
            Converter_Int32ToBytes(data, &index, damage);
            Converter_Int32ToBytes(data, &index, health);
            Converter_Int32ToBytes(data, &index, speed);

            SDLNet_TCP_Send(client[j].socket, data, 128);
            //printf("Stats was sent to '%s'\n", client[j].name);

            x+=200;
        }
    }
}

//Synchronizes servers objects to the clients.
void SendSyncObjects(Scene* scene){
    int i,j;
    printf("Sending SyncObjects.\n");
    for(i = 0; i < scene->objCount; i++)
    {
        unsigned char msg[128];
        int index=1;
        msg[0]=5;
        Converter_Int32ToBytes(msg, &index, scene->objects[i].obj_id);
        Converter_Int32ToBytes(msg, &index, scene->objects[i].rect.x);
        Converter_Int32ToBytes(msg, &index, scene->objects[i].rect.y);
        msg[index++]=scene->objects[i].type;
        Converter_Int32ToBytes(msg, &index, (int)strlen(scene->objects[i].name));

        //printf("Sent object with namelength = %d\n", strlen(scene->objects[i].name));


        for(j = 0; j < strlen(scene->objects[i].name); j++)
        {
            msg[index++] = scene->objects[i].name[j];
        }

        for(j = 0; j < N_CLIENTS; j++)
        {
            if(client[j].status && scene->objects[i].obj_id != client[j].playerId)
            {
                SDLNet_TCP_Send(client[j].socket, msg, 128);
                //printf("object id %d was sent to id:'%d' name'%s'\n",scene->objects[i].obj_id, j,client[j].name);
                //printf("X = '%d' , y = '%d'\n", scene->objects[i].rect.x, scene->objects[i].rect.y);
            }
        }
    }
}

//Send the players classes to all clients.
void SendClassesFinal()
{
    for(int i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status)
        {
            unsigned char buffer[128];
            int index = 0;
            buffer[index++] = NET_PLAYER_CLASS_FINAL;
            Converter_Int32ToBytes(buffer, &index, client[i].playerId);
            buffer[index++] = client[i].pClass;

            for(int j = 0; j < N_CLIENTS; j++)
            {
                if(client[j].status && client[j].playerId != client[i].playerId)
                {
                    SDLNet_TCP_Send(client[j].socket, buffer, 128);
                }
            }
        }

    }

}

void SendRemoveObject(int objId)
{
    unsigned char msg[128];
    int index=1, i;
    msg[0]=6;
    Converter_Int32ToBytes(msg, &index, objId);

    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true)
            SDLNet_TCP_Send(client[i].socket, msg, 128);
    }

}

//Send the players unique ID to the client.
void SendPlayerId(int PlayerId){
    unsigned char msg[128];
    int index=1,i;
    msg[0]=7;
    //printf("Player id = '%d'\n",PlayerId);
    Converter_Int32ToBytes(msg, &index, PlayerId);
    //printf("debug1\n");
    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId == PlayerId)
            SDLNet_TCP_Send(client[i].socket, msg, 128);
    }


}

void SendWeapon(int playerId, ItemType_T type)
{
    char data[128];
    int index = 0;

    data[index++] = NET_PLAYER_WEAPON;

    data[index++] = type;

    for(int i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId == playerId)
        {
            SDLNet_TCP_Send(client[i].socket, data, 128);
            break;
        }
    }
}

void SendArmor(int playerId, int amount)
{
    unsigned char data[128];
    int index = 0;

    data[index++] = NET_PLAYER_ARMOR;
    Converter_Int32ToBytes(data, &index, amount);


    for(int i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId == playerId)
        {
            SDLNet_TCP_Send(client[i].socket, data, 128);
            break;
        }
    }

}

void SendAmmo(int playerId, int amount)
{
    unsigned char data[128];
    int index = 0;

    data[index++] = NET_PLAYER_AMMO;
    Converter_Int32ToBytes(data, &index, amount);


    for(int i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true && client[i].playerId == playerId)
        {
            SDLNet_TCP_Send(client[i].socket, data, 128);
            break;
        }

    }

}

void SendLobbyPlayer(char* playerName, char pClass, int playerId)
{
    unsigned char data[128];
    int index = 1,i;
    int length = (int)strlen(playerName);

    data[0] = NET_LOBBY_PLAYER;

    Converter_Int32ToBytes(data, &index, length);

    for(i = 0; i < length; i++)
    {
        data[index + i] = playerName[i];
        //printf("Packing down name at index %d\n", index + i);
    }
    index += length;

    data[index++] = pClass;

    //printf("Sending player with name %s to clients\n", playerName);

    for(i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status == true)
        {
            //printf("Comparing player ids...\n");
            SDLNet_TCP_Send(client[i].socket, data, 128);
            /*
            if(client[i].playerId != playerId)
            {
                SDLNet_TCP_Send(client[i].socket, data, 128);
                printf("Sent player to %s\n", client[i].name);
            }*/
        }
    }

}

void SendPlayerHealth(int playerId, int health)
{
    unsigned char data[128];
    int index = 0,i;

    data[index++] = NET_PLAYER_HEALTH;
    Converter_Int32ToBytes(data, &index, health);
    //TODO: kanske lägga till så att man skickar ut health till andra spelare också.

    for(i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].playerId == playerId)
        {
            //printf("Sent player health\n");
            SDLNet_TCP_Send(client[i].socket, data, 128);
            //client[i].socket.close();
            break;
        }
    }
}

void SendPlayerClass(PlayerClass_T pClass, char* name)
{
    unsigned char data[128];
    int index = 0;
    int nameLength = (int)strlen(name);

    data[index++] = NET_PLAYER_CLASS_REC;
    Converter_Int32ToBytes(data, &index, nameLength);

    for(int i = 0; i < nameLength; i++)
    {
        data[index++] = name[i];
    }
    data[index++] = pClass;

    for(int i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status)
        {
            printf("Player class sent\n");
            SDLNet_TCP_Send(client[i].socket, data, 128);
        }
    }
}

void RecvPlayerClass(unsigned char data[], Scene* scene)
{
    int playerId, index = 1;
    PlayerClass_T pClass;

    playerId = Converter_BytesToInt32(data, &index);
    pClass = data[index++];

    printf("PlayerId = %d\n", playerId);

    for(int i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].playerId == playerId)
        {
            client[i].pClass = pClass;
            SendPlayerClass(pClass, client[i].name);
            break;
        }
    }
}


void RecvPlayerShoot(unsigned char data[], Scene* scene)
{
    int index = 1;
    int shooterId, x, y, angle, damage, speed;
    //bulletType_T bType;

    shooterId = Converter_BytesToInt32(data, &index);
    x = Converter_BytesToInt32(data, &index);
    y = Converter_BytesToInt32(data, &index);
    angle = Converter_BytesToInt32(data, &index);
    damage = Converter_BytesToInt32(data, &index);
    speed = Converter_BytesToInt32(data, &index);

    //printf("Received a shoot with x = '%d' y = '%d' speed = '%d'\n",x, y, speed);

    GameObject bullet = CreateBullet(scene->nextId++, x, y, damage, angle, speed, BULLET_PLAYER);
    AddObject(scene, bullet, false); //net är false eftersom att bullet är ett speciellt objekt.
    SendBullet(bullet);

}

void RecvPlayerPos(unsigned char data[], Scene* scene){
    int index = 1,i;
    int playerId, x, y, angle;

    playerId = Converter_BytesToInt32(data, &index);
    x = Converter_BytesToInt32(data, &index);
    y = Converter_BytesToInt32(data, &index);
    angle = (float)Converter_BytesToInt32(data, &index);

    for(i = 0; i < scene->objCount; i++)
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

void RecvPlayerReady(unsigned char data[], Scene *scene)
{
    int index = 1,i,j;
    int playerId = Converter_BytesToInt32(data, &index);

    index = 0;
    data[index++] = NET_PLAYER_READY;


    for(i = 0; i < N_CLIENTS; i++)
    {
        if(client[i].status && client[i].playerId == playerId)
        {
            printf("Player %s is ready\n", client[i].name);

            Converter_Int32ToBytes(data, &index, (int)strlen(client[i].name));

            for(j = 0; j < strlen(client[i].name); j++)
            {
                data[index++] = client[i].name[j];
            }
            data[index++] = '\0';
            //printf("test\n");

            for(j = 0; j < N_CLIENTS; j++) //Sends to all other clients that the player who sent ready is ready.
            {
                if(client[j].status)
                {
                    SDLNet_TCP_Send(client[j].socket, data, 128);
                }
            }
            client[i].ready = true;
            break;
        }
    }
}
