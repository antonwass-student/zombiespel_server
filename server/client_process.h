#ifndef __SDL_net__client_process__
#define __SDL_net__client_process__

#ifdef __APPLE__
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

#include "client_process.h"
#include "server_structs.h"
#include "net_msgs.h"

void* client_process(void* arg);

#endif /* defined(__SDL_net__client_process__) */
