//
//  server_structs.c
//  spel_server
//
//  Created by cristian araya on 12/05/15.
//  Copyright (c) 2015 spel_server. All rights reserved.
//

#ifdef __APPLE__
#include <SDL2/SDL.h>
#include "SDL2_net/SDL_net.h"

#elif __linux
#include "SDL2/SDL_net.h"
#endif

#define N_CLIENTS 2
#include <stdio.h>
#include "server_structs.h"


Connections client[N_CLIENTS];