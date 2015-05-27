#if 0
#!/bin/sh
gcc -Wall `sdl-config --cflags` tcps.c -o tcps `sdl-config --libs` -lSDL_net -pthread

exit
#endif

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

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
//#include "server_update.h"

int nextId = 0;


int main(int argc, char **argv)
{
    int deltaTime = SDL_GetTicks();

    bool lobbyReady = false;
    bool anyoneHere = false;
    //GameObject objects[100];
    Scene level;
    GameObject newObject;

    level.obj_mutex = SDL_CreateMutex();
    level.objCount = 0;
    level.nextId = 100;

    poolInit();
    connectionInit();
    pthread_t listener;
    printf("Starting listener thread.\n");
    pthread_create(&listener,NULL, &client_handle, (void*)&level);
    newObject=CreateZombie(0,0, level.nextId++);
    AddObject(&level, newObject, false);
    //SendNewObject(newObject.obj_id, newObject.x, newObject.y, OBJECT_ZOMBIE_NORMAL);
    int i;
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

        if (deltaTime < 17){
            SDL_Delay(17-deltaTime);
            //printf("%d\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    printf("Everyone is ready!\n");
    printf("Spawning objects...\n");

    SendGameStart();
    SendPlayerStats(&level);

    SendSyncObjects(&level);
    SendClassesFinal();


    //LoadLevel(&level); //Ladda in banans objekt
    //newObject=CreateZombieSpitter(2750,4800, level.nextId++);
    //AddObject(&level, newObject, true);

    //newObject=CreateZombieSpitter(2900,4800, level.nextId++);
    //AddObject(&level, newObject, true);

    newObject=CreateZombieSpitter(3100,4000, level.nextId++);
    AddObject(&level, newObject, true);

    newObject = CreateMedkit(3100,4800, level.nextId++);
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



    printf("_______________\n***Game started****\n");

    while (1)
    { //GAMELOOP
        int netLoad;
        netLoad = readPool(&level);//Läser nätverksmeddelanden från klienter

        Update(&level); //Uppdaterar alla objekt på servern

        deltaTime = SDL_GetTicks() - deltaTime;
        if (deltaTime < 17){
            SDL_Delay(17-deltaTime);
            if(17-deltaTime < 5)
                printf("***Warning***\n high server load = %d\n**********\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    printf("Server stopped\n");
    pthread_join(listener, NULL);
    return EXIT_SUCCESS;
}
