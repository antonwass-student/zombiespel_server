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

int Update(Scene* scene);

int nextId = 0;


int main(int argc, char **argv)
{
    int deltaTime = SDL_GetTicks();

    bool lobbyReady = false;
    //bool anyoneHere = false;
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

    int i;
    printf("Lobby is open.\nWaiting for connection\n_______\n");

    while(!lobbyReady) //Lobby loop
    {
        readPool(&level);

        for(i = 0; i < N_CLIENTS; i++)
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
    
    newObject=CreateZombie(611,3126, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(611,3441, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(846,3476, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(1026,3476, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(1381,3526, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(1356,3761, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(1281,4191, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(1941,4206, level.nextId++);
    AddObject(&level, newObject, true);
    
    newObject=CreateZombie(2166,3766, level.nextId++);
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
    //printf("Server stopped\n");
    //pthread_join(listener, NULL);
    return EXIT_SUCCESS;
}
