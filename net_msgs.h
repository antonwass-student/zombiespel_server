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
void connectionInit();
int Converter_BytesToInt32(unsigned char data[], int* index);
int Converter_Int32ToBytes(unsigned char data[], int* size, int value);
int AddToPool(char* msg);
void SendObjectPos(int objId, int x, int y, int angle);//MSG 2
void SendNewObject(int objId, int x, int y, objectType_t type); //MSG 5
void SendRemoveObject(int objId); //MSG 6
void SyncObjects(int clientID);
void SendPlayerId(int PlayerId, int i); //MSG 7
void RecvPlayerPos(unsigned char data[], Scene* scene);

#endif /* defined(__SDL_net__net_msgs__) */
