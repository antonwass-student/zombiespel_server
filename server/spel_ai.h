#ifndef q_spel_ai_h
#define q_spel_ai_h

void Zombie_UseBrain(Scene* scene, GameObject* zombie, int index, bool netUpdate);
int Zombie_Attack(GameObject* zombie, Scene* scene);
int Zombie_Shoot(GameObject* zombie, Scene* scene);
SDL_Rect* FindPlayer(Scene* scene, GameObject* zombie, int range);

#endif
