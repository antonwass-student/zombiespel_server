//
//  client_process.c
//  SDL_net
//
//  Created by cristian araya on 08/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//
#ifdef __APPLE__
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

#include "client_process.h"
#include "server_structs.h"
#include "net_msgs.h"

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
            AddToPool(buffer);
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