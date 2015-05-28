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
    //char msg[128];
    int i;
    for (i=1; i<128; i++) {
        if (recvPool.queue[n][i]== '\n' || recvPool.queue[n][i]== '\0' ) {
            break;
        }
        printf("%c",recvPool.queue[n][i]);
    }
    printf("\n");

}

void player_shoot(int n, Scene* scene){
    //int playerId, int angle
    //int id = recvPool.queue[n][1];
    //int angle = recvPool.queue[n][2];
}

int readPool(Scene *scene){
    int i, size;
    if(recvPool.size > 100)
        printf("****Warning****\nHigh NetworkLoad: %d/128\n********", recvPool.size);

    for(i=0;i<recvPool.size;i++){
        switch (recvPool.queue[i][0]) {
            case 1: //chatt
                printf("%s",recvPool.queue[i]);
                chat_msg(i);
                break;
            case 2: //chatt
                printf("Message with id 2 was received\n");
                break;
            case 3: //player_move
                RecvPlayerPos(recvPool.queue[i], scene);
                break;
            case NET_PLAYER_SHOOT: //player_shoot
                //printf("Received shoot from player\n");
                RecvPlayerShoot(recvPool.queue[i], scene);
                break;
            case 8: //send player_name
                printf("Message with ID 8 was received.\n");
                break;
            case 11:
                RecvPlayerReady(recvPool.queue[i], scene);
                break;
            case NET_PLAYER_CLASS:
                RecvPlayerClass(recvPool.queue[i], scene);
                printf("Received player class\n");
                break;
            default:
                break;
        }
    }
    size = recvPool.size;
    recvPool.size = 0;
    return size;

}
