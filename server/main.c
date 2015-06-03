#if 0
#!/bin/sh
gcc -Wall `sdl-config --cflags` tcps.c -o tcps `sdl-config --libs` -lSDL_net -pthread

exit
#endif

#ifdef __APPLE__
#include "SDL2/SDL.h"
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL.h"
#include "SDL2/SDL_net.h"
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>

#include "spel_objects.h"
#include "server_structs.h"
#include "client_handler.h"
#include "net_msgs.h"
#include "pool_reader.h"
#include "spel_ai.h"
#include "server_update.h"


int nextId = 0;


int main(int argc, char **argv)
{
    int deltaTime = SDL_GetTicks();
    bool lobbyReady = false;
    //bool anyoneHere = false;
    //GameObject objects[100];
    bool netUpdate=false;
    int netUpdateTimer=12;
    int netUpdateRate=12;
    Scene level;
    GameObject newObject;

    level.obj_mutex = SDL_CreateMutex();
    level.objCount = 0;
    level.nextId = 100;

    connectionInit();
    pthread_t listener;
    printf("Starting listener thread.\n");
    pthread_create(&listener,NULL, &client_handle, (void*)&level);

    for(int i=0;i<15;i++) //Creates 15 zombie spitters
    {
        newObject=CreateZombieSpitter(1500 + 200*i,4000, level.nextId++);
        AddObject(&level, newObject, false);
    }
    for(int i=0;i<15;i++) //Creates 15 zombies
    {
        newObject=CreateZombieSpitter(1500 + 200*i,3800, level.nextId++);
        AddObject(&level, newObject, false);
    }


    printf("Lobby is open.\nWaiting for connection\n_______\n");

    while(!lobbyReady) //Lobby loop
    {
        readPool(&level);

        for(int i = 0; i < N_CLIENTS; i++)
        {
            if(client[i].status)
            {
                if(client[i].ready)
                {
                    lobbyReady = true;
                }
                else
                {
                    lobbyReady = false;
                    break;
                }

            }
        }

        if (deltaTime < 17)
        {
            SDL_Delay(17-deltaTime);
            //printf("%d\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    printf("Everyone is ready!\n");
    printf("Spawning objects...\n");

    //Sends a message to clients that the game is starting
    SendGameStart();
    //Sends a message to each player with their class stats (health, armor, damage, speed)
    SendPlayerStats(&level);

    //Sends all objects on the server to the clients.
    SendSyncObjects(&level);
    //Send all the players classes to the players (so that each client knows which class the other players have)
    SendClassesFinal();

    //Creates objects on the server. With the third parameter of AddObject set to true means that they will also be sent to the clients.
    newObject = CreateMedkit(3200,4900, level.nextId++);
    AddObject(&level, newObject, true);

    newObject = CreateAmmo(3100,4800, level.nextId++);
    AddObject(&level, newObject, true);

    newObject = CreateArmor(3000,4800, level.nextId++);
    AddObject(&level, newObject, true);

    newObject = CreateShotgun(3200,4800, level.nextId++);
    AddObject(&level, newObject, true);

    newObject = CreateRevolver(3300,4800, level.nextId++);
    AddObject(&level, newObject, true);

    newObject = CreateMachineGun(3400,4800, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(2500,4000, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(2600,4000, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(2496,1406, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(2806,1406, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(2656,2156, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(2016,1941, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(1496,1696, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(866,1946, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(831,2216, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(1181,2541, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(1176,2741, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(921,2786, level.nextId++);
    AddObject(&level, newObject, true);

    newObject=CreateZombie(826,3031, level.nextId++);
    AddObject(&level, newObject, true);

    printf("_______________\n***Game started****\n");

    //Main gameloop
    while (1)
    {
        int netLoad;
        //This module is used to limit the messages sent to the client. 5 messages per second
        netUpdateTimer--;
        if(netUpdateTimer==0)
        {
            netUpdate=true;
            netUpdateTimer=netUpdateRate;
        }
        netLoad = readPool(&level);//Reads all the netmessages from the clients

        Update(&level,netUpdate); //Updates all the objects on the server.

        //Limits the updaterate to 60 times per second
        deltaTime = SDL_GetTicks() - deltaTime;
        if (deltaTime < 17)
        {
            SDL_Delay(17-deltaTime);
            if(17-deltaTime < 5)
                printf("***Warning***\n high server load = %d\n**********\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
        netUpdate=false;
    }
    return EXIT_SUCCESS;
}
