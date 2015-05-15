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

int nextId = 0;


int main(int argc, char **argv)
{
    int deltaTime = SDL_GetTicks();
    //GameObject objects[100];
    Scene level;
    GameObject newObject;

    level.obj_mutex = SDL_CreateMutex();
    level.objCount = 0;
    level.nextId = 100;

    poolInit();
    pthread_t listener;
    pthread_create(&listener,NULL, &client_handle, (void*)&level);
    //sleep(5);
    newObject=CreateZombie(3000,600, level.nextId++);
    AddObject(&level, newObject);
    //SendNewObject(newObject.obj_id, newObject.x, newObject.y, OBJECT_ZOMBIE_NORMAL);
    int i;
    printf("Loopen startar...\n");
    while (1) {
        for(i=0;i<level.objCount;i++)
        {
            if(level.objects[i].type==OBJECT_ZOMBIE_NORMAL)
            {
                //level.objects[i].x+=10;
                //SendObjectPos(objects[i].obj_id, objects[i].x, objects[i].y, 0);
                //printf("Object moved\n");
            }
        }
        readPool(&level);
        deltaTime = SDL_GetTicks() - deltaTime;
        if (deltaTime < 17){
            SDL_Delay(17-deltaTime);
            //printf("%d\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    pthread_join(listener, NULL);
    return EXIT_SUCCESS;
}
