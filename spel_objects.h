//
//  spel_objects.h
//  SDL_net
//
//  Created by cristian araya on 08/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//

#ifndef __SDL_net__spel_objects__
#define __SDL_net__spel_objects__

#include <stdio.h>
#include "server_structs.h"

int AddObject(GameObject objects[], GameObject object, int *size);
int RemoveObject(GameObject objects[], int id, int *size);
GameObject CreateZombie(int x, int y, int id);
GameObject CreatePlayer(int x, int y, int id);

#endif /* defined(__SDL_net__spel_objects__) */
