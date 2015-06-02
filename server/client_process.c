#ifdef __APPLE__
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

#include "client_process.h"
#include "server_structs.h"
#include "net_msgs.h"

//Function that handles each connected client.
void* client_process(void* arg)
{
        SDLNet_SocketSet set;
        char buffer[512]={0};
        int quit = 0;
        int i = *(int*) arg;
        set = SDLNet_AllocSocketSet(1);
        SDLNet_TCP_AddSocket(set, client[i].socket);
        printf("socket added");
        while (!quit)
        {
            if(client[i].socket != NULL){
                int result = SDLNet_TCP_Recv(client[i].socket, buffer, 511);
                if(SDLNet_CheckSockets(set, 20000) < 0)
                {
                    printf("Socket receive error! %d \n",i);
                    SDLNet_TCP_Close(client[i].socket);
                    client[i].status=NULL;
                    client[i].status=false;

                    quit=true;
                }
                if (result <= 0)
                {
                    SDLNet_TCP_Close(client[i].socket);
                    client[i].status = false;
                    quit = true;
                    printf("kick out player id %d\n",client[i].playerId);

                    break;
                    //AddToPool(buffer);
                }
                else{
                    AddToPool(buffer);
                }
            }
        }

    return EXIT_SUCCESS;
}

