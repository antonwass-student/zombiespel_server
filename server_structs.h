//
//  server_structs.h
//  SDL_net
//
//  Created by cristian araya on 08/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//

#ifndef __SDL_net__server_structs__
#define __SDL_net__server_structs__

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define N_CLIENTS 2
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct  {
    char name[20];
    int playerId;
    TCPsocket socket;
    bool status; //för återanvändning
    pthread_t tid;
}Connections;

typedef enum {
    OBJECT_ZOMBIE_NORMAL,
    OBJECT_WALL,
    OBJECT_MEDKIT,
    OBJECT_PLAYER
    
}objectType_t;

typedef struct{
    objectType_t type;
    int x;
    int y;
    int obj_id;
    
}GameObject;

typedef struct{
    char queue[128][512]; //Kö från klienterna till main
    int size;

}msg_stack;

Connections client[N_CLIENTS];


#endif /* defined(__SDL_net__server_structs__) */
