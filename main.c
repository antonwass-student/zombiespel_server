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
#include "net_msgs.h"
#include "client_handler.h"
#include "spel_objects.h"
#include "server_structs.h"
#include "client_process.h"




int main(int argc, char **argv)
{
    RecvStack.size = 1;
    int id=0, objectCount=0;
    int deltaTime = SDL_GetTicks();
    GameObject objects[100];
    GameObject newObject;
    pthread_t listener;
    pthread_create(&listener,NULL, &client_handle, (void*)objects);
    sleep(10);
    //pthread_create(&broadcaster, NULL, &SendToAll, NULL);
    newObject=CreateZombie(0,0,id++);
    AddObject(objects, newObject, &objectCount);
    SendNewObject(newObject.obj_id, newObject.x, newObject.y, OBJECT_ZOMBIE_NORMAL);
    int i;
    sleep(2);
    while (1) {
        for(i=0;i<objectCount;i++)
        {
            if(objects[i].type==OBJECT_ZOMBIE_NORMAL)
            {
                objects[i].x+=10;
                SendObjectPos(objects[i].obj_id, objects[i].x, objects[i].y, 0);
                printf("Object moved\n");
            }

        }
        deltaTime = SDL_GetTicks() - deltaTime;
        if (deltaTime < 17){
            SDL_Delay(17-deltaTime);
            printf("%d\n",17-deltaTime);
        }
        deltaTime=SDL_GetTicks();
    }
    pthread_join(listener, NULL);
    return EXIT_SUCCESS;
}
