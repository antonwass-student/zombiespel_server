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

void player_move(int n, GameObject objects[100]){
    //int playerId, char vertical, char horizontal
    int id = recvPool.queue[n][1];
    char vertical = recvPool.queue[n][2];
    char horizontal =recvPool.queue[n][3];

    if (horizontal== 'a')
        objects[id].x-=10;
    else if (horizontal == 'd')
        objects[id].x+=10;
    else
        ;//failade
    
    if (vertical == 's')
        objects[id].y-=10;
    else if (vertical == 'w')
        objects[id].y+=10;
    else
        ;//failade
}

void player_shoot(int n, GameObject objects[100]){
    //int playerId, int angle
    int id = recvPool.queue[n][1];
    int angle = recvPool.queue[n][2];
}

void player_name(int n){
    
}

void readPool(GameObject objects[100]){
    int i;
    for(i=0;i<recvPool.size;i++){
        switch (recvPool.queue[i][0]) {
            case 1: //chatt
                printf("%s",recvPool.queue[i]);
                chat_msg(i);
                break;
            case 3: //player_move
                player_move(i, objects);
                break;
            case 4: //player_shoot
                player_shoot(i,objects);
                break;
            case 8: //send player_name
                printf("Du fått medd 8\n");
                player_name(i);
                
                break;
            default:
                break;
        }
    }
    
}