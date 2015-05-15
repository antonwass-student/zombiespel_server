//
//  pool_reader.c
//  spel_server
//
//  Created by cristian araya on 12/05/15.
//  Copyright (c) 2015 spel_server. All rights reserved.
//
#ifdef __APPLE__
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include "pool_reader.h"
#include "net_msgs.h"
#include "server_structs.h"
#include "spel_objects.h"


void chat_msg(int n){
    //string msg, string sender (ändra till playerId och sätta först??)
    char msg[512];
    int i;
    for (i=1; i<512; i++) {
        if (recvPool.queue[n][i]== '\n' || recvPool.queue[n][i]== '\0' ) {
            break;
        }
        printf("%c",recvPool.queue[n][i]);
    }
    printf("\n");

}

void player_shoot(int n, Scene* scene){
    //int playerId, int angle
    int id = recvPool.queue[n][1];
    int angle = recvPool.queue[n][2];
}

void player_name(int n){

}

void readPool(Scene *scene){
    int i;
    for(i=0;i<recvPool.size;i++){
        switch (recvPool.queue[i][0]) {
            case 1: //chatt
                printf("%s",recvPool.queue[i]);
                chat_msg(i);
                break;
            case 3: //player_move
                //player_move(recvPool.queue[i], objects);
                RecvPlayerPos(recvPool.queue[i], scene);
                break;
            case 4: //player_shoot
                player_shoot(i, scene);
                break;
            case 8: //send player_name
                printf("Du fått medd 8\n");
                player_name(i);

                break;
            default:
                break;
        }
    }
    recvPool.size = 0;

}
