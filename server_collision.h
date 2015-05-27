#ifndef SERVER_COLLISION_H_INCLUDED
#define SERVER_COLLISION_H_INCLUDED

#include <SDL2/SDL.h>
#include "server_structs.h"

int GetDistance(SDL_Rect obj1, SDL_Rect obj2);
bool MoveObject(GameObject* movingObject, Scene* scene, int speedX, int speedY, int objectIndex);
int NewDamage(GameObject* NPC, GameObject* Player);
void ProximityCheck(GameObject* obj1, GameObject* obj2, int obj1_index,int obj2_index, Scene* scene);
void CollisionHandler(GameObject* collider1, GameObject* collider2, int c1_index, int c2_index, Scene* scene);

#endif // SERVER_COLLISION_H_INCLUDED
