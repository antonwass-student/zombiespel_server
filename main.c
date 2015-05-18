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
    AddObject(&level, newObject);
    //SendNewObject(newObject.obj_id, newObject.x, newObject.y, OBJECT_ZOMBIE_NORMAL);
    int i;
    printf("Lobby starting...\n");

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
                }

            }
        }

        if (deltaTime < 17){
            SDL_Delay(17-deltaTime);
            //printf("%d\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    printf("Everyone is ready:\n");
    printf("Game starting...\n");

    SendGameStart();


    //LoadLevel(&level); //Ladda in banans objekt
    newObject=CreateZombie(3000,600, level.nextId++);
    AddObject(&level, newObject);
    //LoadPlayers(&level); //Ladda in spelare i banan


    while (1)
    { //GAMELOOP
        readPool(&level);//Läser nätverksmeddelanden från klienter

        //Update(&level); //Uppdaterar alla objekt på servern

        deltaTime = SDL_GetTicks() - deltaTime;
        if (deltaTime < 17){
            SDL_Delay(17-deltaTime);
            //printf("%d\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    printf("Server stopped\n");
    pthread_join(listener, NULL);
    return EXIT_SUCCESS;
}
