//
//  net_msgs.h
//  SDL_net
//
//  Created by cristian araya on 06/05/15.
//  Copyright (c) 2015 project. All rights reserved.
//

#ifndef __SDL_net__net_msgs__
#define __SDL_net__net_msgs__

#include <stdio.h>
#include "server_structs.h"
extern msg_stack recvPool;

void poolInit();
int Converter_BytesToInt32(char data[], int* index);
int Converter_Int32ToBytes(char data[], int* size, int value);
int AddToPool(char* msg);
void SendObjectPos(int objId, int x, int y, int angle);
void SendNewObject(int objId, int x, int y, objectType_t type);
void SendRemoveObject(int objId);
void SyncObjects(int clientID);
void SendPlayerId(int PlayerId, int i);
void RecvPlayerPos(char data[], Scene* scene);

#endif /* defined(__SDL_net__net_msgs__) */
