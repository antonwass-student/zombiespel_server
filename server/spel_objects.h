#ifndef __SDL_net__spel_objects__
#define __SDL_net__spel_objects__

#include "server_structs.h"

int AddObject(Scene* scene, GameObject object, bool net);
int RemoveObject(Scene* scene, int id);
GameObject CreateZombieSpitter(int x, int y, int id);
GameObject CreateZombie(int x, int y, int id);
GameObject CreatePlayer(int x, int y, int id, char* name);
GameObject CreateBullet(int id, int x, int y, int damage, int direction, int velocity, bulletType_T bType);
GameObject CreateMedkit(int x, int y, int id);
GameObject CreateAmmo(int x, int y, int id);
GameObject CreateArmor(int x, int y, int id);
GameObject CreateShotgun(int x, int y, int id);
GameObject CreateMachineGun(int x, int y, int id);
GameObject CreateRevolver(int x, int y, int id);

#endif /* defined(__SDL_net__spel_objects__) */