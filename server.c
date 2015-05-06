#if 0
#!/bin/sh
gcc -Wall `sdl-config --cflags` tcps.c -o tcps `sdl-config --libs` -lSDL_net -pthread

exit
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#define N_CLIENTS 2


//#include "SDL2_net/SDL_net.h" //mac
#include "SDL2/SDL_net.h"

//hejhej

typedef enum {
    OBJECT_ZOMBIE_NORMAL,
    OBJECT_WALL,
    OBJECT_MEDKIT,
    OBJECT_PLAYER
}
objectType_t;

typedef struct{
    objectType_t type;
    int x, y, id;
}GameObject;

typedef struct  {
    char name[20];
    int playerId;
    TCPsocket socket;
    bool status; //för återanvändning
    pthread_t tid; // istället för pthread_t tid[N_CLIENTS]; //trådarray för klienter
}Connections;

Connections client[N_CLIENTS];
GameObject objects[100];
char queue[128][512]; //Kö från klienterna till main
int id=0, objectCount=0;
pthread_mutex_t object_mutex=PTHREAD_MUTEX_INITIALIZER;




int Converter_BytesToInt32(char data[], int* index){

    int value;

    value = ((int)data[*index] << 24) + ((int)data[*index + 1] << 16)
    + ((int)data[*index + 2] << 8) + ((int)data[*index + 3]);

    *index += 4;

    return value;
}

int Converter_Int32ToBytes(char data[], int* size, int value)
{
    data[*size] = value >> 24;
    data[*size + 1] = value >> 16;
    data[*size + 2] = value >> 8;
    data[*size + 3] = value;
    *size += 4;

    return 0;
}

void SendObjectPos(int objId, int x, int y, int angle){
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

void SendNewObject(int objId, int x, int y, objectType_t type){
    char msg[512];
    int index=1, i;
    msg[0]=5;
    Converter_Int32ToBytes(msg, &index, objId);
    Converter_Int32ToBytes(msg, &index, x);
    Converter_Int32ToBytes(msg, &index, y);
    msg[index++]=type;

    for(i=0;i<N_CLIENTS;i++)
    {
        if(client[i].status == true)
            SDLNet_TCP_Send(client[i].socket, msg, 512);
    }

}

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
        if(objects[i].id==id)
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

GameObject CreateZombie(int x, int y, int id)
{
    GameObject object;
    object.x=x;
    object.y=y;
    object.id=id;
    object.type=OBJECT_ZOMBIE_NORMAL;
    return object;
}

GameObject CreatePlayer(int x, int y, int id)
{
    GameObject object;
    object.x=x;
    object.y=y;
    object.id=id;
    object.type=OBJECT_PLAYER;
    return object;
}


/*void* SendToAll(void* arg){
    bool quit = 0;
    int i;
    while(!quit){
        for(i=0;i<N_CLIENTS;i++)
        {
            if(client[i].status == true)
            SDLNet_TCP_Send(client[i].socket, msg, 512);
        }
    }
}*/


void* client_process(void* arg)
{
    char buffer[512];
    int quit = 0;
    int i = *(int*) arg;
    while (!quit)
    {
        //SDL_Delay(10);

        if (SDLNet_TCP_Recv(client[i].socket, buffer, 512) > 0)
        {

            printf("Client %d says: %s\n", i, buffer);
            SDLNet_TCP_Send(client[i].socket, buffer, sizeof(buffer));
            if(strcmp(buffer, "exit") == 0)	/* Terminate this connection */
            {
                quit = 1;
                printf("Terminate connection\n");
            }
        }
    }
    SDLNet_TCP_Close(client[i].socket);
    client[i].status = false;
    return EXIT_SUCCESS;
}

void* client_handle(void* arg){
    TCPsocket sd, csd, tmp; /* Socket descriptor, Client socket descriptor */
    IPaddress ip, *remoteIP;
    int listening = 1, i=0;

    if (SDLNet_Init() < 0)
    {
        fprintf(stderr, "SDLNet_Init: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    /* Resolving the host using NULL make network interface to listen */
    if (SDLNet_ResolveHost(&ip, NULL, 2000) < 0)
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    /* Open a connection with the IP provided (listen on the host's port) */
    if (!(sd = SDLNet_TCP_Open(&ip)))
    {
        fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
        exit(EXIT_FAILURE);
    }

    while (listening)
    {
        /* Our server should always accept new connections but close them if server full */
        if ((csd = SDLNet_TCP_Accept(sd)))
        {
            /* Now we can communicate with the client using csd socket
             * sd will remain opened waiting other connections */

            /* Get the remote address */
            if ((remoteIP = SDLNet_TCP_GetPeerAddress(csd)))
            /* Print the address, converting in the host format */
                printf("Host connected: %x %d\n", SDLNet_Read32(&remoteIP->host), SDLNet_Read16(&remoteIP->port));
            else
                fprintf(stderr, "SDLNet_TCP_GetPeerAddress: %s\n", SDLNet_GetError());

            int sockets_available=1;
            if (sockets_available)
            {
                for(i=0; i<N_CLIENTS;i++){
                    if(client[i].status == false){
                        client[i].socket = csd;
                        client[i].status = true;
                        GameObject player=CreatePlayer(0,0, id++);
                        AddObject(objects, player, &objectCount);
                        pthread_create(&client[i].tid, NULL, &client_process, &i);
                        break;//hittat en ledig plats
                    }
                    else
                        continue;
                }
            }

            else
            {
                tmp = csd;
                printf("Server full. Connection denied\n");
                SDLNet_TCP_Send(tmp, "Server full. Connection denied", 30);
                SDLNet_TCP_Send(tmp, "Turn you off baby", 20);
                SDLNet_TCP_Close(tmp);
            }
        }

    }


    //SDLNet_TCP_Close(csd);
    SDLNet_TCP_Close(sd);
    SDLNet_Quit();

    return EXIT_SUCCESS;
}


int main(int argc, char **argv)
{
    GameObject newObject;
    pthread_t listener, broadcaster;
    pthread_create(&listener,NULL, &client_handle, NULL);
    sleep(10);
    //pthread_create(&broadcaster, NULL, &SendToAll, NULL);
    newObject=CreateZombie(0,0,id++);
    AddObject(objects, newObject, &objectCount);
    SendNewObject(newObject.id, newObject.x, newObject.y, OBJECT_ZOMBIE_NORMAL);
    int i;
    sleep(2);
    while (1) {
        pthread_mutex_lock(&object_mutex);
        for(i=0;i<objectCount;i++)
        {
            if(objects[i].type==OBJECT_ZOMBIE_NORMAL)
            {
                objects[i].x+=10;
                SendObjectPos(objects[i].id, objects[i].x, objects[i].y, 0);
                printf("Object moved\n");
            }

        }
        pthread_mutex_unlock(&object_mutex);
        sleep(1);
    }
    pthread_join(listener, NULL);
    return EXIT_SUCCESS;
}
