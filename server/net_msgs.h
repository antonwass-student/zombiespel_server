#ifndef __SDL_net__net_msgs__
#define __SDL_net__net_msgs__

#include <stdio.h>
#include "server_structs.h"
extern msg_stack recvPool;

void poolInit();
void SendGameStart();
void SendPlayerStats();
void connectionInit();
void RecvPlayerShoot(unsigned char data[], Scene* scene);
void SendPlayerHealth(int playerId, int health);
int Converter_BytesToInt32(unsigned char data[], int* index);
int Converter_Int32ToBytes(unsigned char data[], int* size, int value);
int AddToPool(char* msg);
void SendObjectPos(int objId, int x, int y, int angle);//MSG 2
void SendNewObject(int objId, int x, int y, objectType_t type, char* name); //MSG 5
void SendRemoveObject(int objId); //MSG 6
void SendLobbyPlayer(char* playerName, char pClass, int playerId);
void SendSyncObjects(Scene* scene);
void SendPlayerId(int PlayerId); //MSG 7
void RecvPlayerClass(unsigned char data[], Scene* scene);
void RecvPlayerReady(unsigned char data[], Scene *scene);
void RecvPlayerPos(unsigned char data[], Scene* scene);
void SendBullet(GameObject bullet);
void SendWeapon(int playerId, ItemType_T type);
void SendArmor(int playerId, int amount);
void SendAmmo(int playerId, int amount);
void SendClassesFinal();

#endif /* defined(__SDL_net__net_msgs__) */
