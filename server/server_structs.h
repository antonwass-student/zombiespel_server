#ifndef __SDL_net__server_structs__
#define __SDL_net__server_structs__

#ifdef __APPLE__
#include "SDL2_net/SDL_net.h"
#include "SDL2/SDL.h"

#elif __linux
#include "SDL2/SDL_net.h"
#include "SDL2/SDL.h"
#endif


#include <stdbool.h>
#include <pthread.h>

#define M_PI 3.14
#define N_CLIENTS 4

typedef enum{
    NET_CHAT_MSG = 1,
    NET_OBJECT_POS,
    NET_PLAYER_MOVE,
    NET_PLAYER_SHOOT,
    NET_OBJECT_NEW,
    NET_OBJECT_REMOVE,
    NET_PLAYER_ID,
    NET_PLAYER_NAME,
    NET_PLAYER_STATS,
    NET_LOBBY_PLAYER,
    NET_PLAYER_READY,
    NET_GAME_START,
    NET_OBJECT_BULLET,
    NET_PLAYER_HEALTH,
    NET_PLAYER_CLASS,
    NET_PLAYER_CLASS_REC,
    NET_PLAYER_CLASS_FINAL,
    NET_PLAYER_WEAPON,
    NET_PLAYER_AMMO,
    NET_PLAYER_ARMOR
} NetMessages_T;


typedef enum {
    CLASS_SOLDIER,
    CLASS_SCOUT,
    CLASS_TANK,
    CLASS_ENGINEER
} PlayerClass_T;

typedef enum{
    BULLET_ZOMBIE,
    BULLET_PLAYER
}bulletType_T;

typedef enum{
    ZOMBIE_NORMAL,
    ZOMBIE_SPITTER
}NPCtype_T;

typedef enum{
    ITEM_MEDKIT = 7,
    ITEM_AMMO,
    ITEM_WEAPON_1,
    ITEM_WEAPON_2,
    ITEM_WEAPON_3,
    ITEM_ARMOR
}ItemType_T;

typedef enum {
    OBJECT_NPC,
    OBJECT_WALL,
    OBJECT_CAR,
    OBJECT_ITEM,
    OBJECT_PLAYER,
    OBJECT_BULLET,
    OBJECT_ZOMBIE_SPITTER

}objectType_t;

typedef enum {
    AI_ZOMBIE,
    AI_SUPERZOMBIE,
    AI_SPITTER
} AI_T;


typedef struct  {
    char name[20];
    int playerId;
    TCPsocket socket;
    bool status; //for recycling
    bool ready;
    pthread_t tid;
    PlayerClass_T pClass;
    bool targetIsAlive;
}Connections;

typedef struct{
    int health, ammo, speed, damage, armor, ammoTotal;
    PlayerClass_T pClass;
    bool alive;
}PlayerInfo;

typedef struct{
    ItemType_T type;
    int amount;
}ItemInfo;

typedef struct{
    int velocity;
    int timetolive;
    int damage;
    double angle;
    bulletType_T type;
}BulletInfo;

typedef struct{
    AI_T ai;
    int health;
    int speed;
    int damage;
    int detectRange;
    float atkCd;
    int attackRange;
    int atkTimer;
    int bulletSpeed;
    SDL_Rect* target;
    bool* targetIsAlive;
    int fireRate;
    int fireCount;
    int tsCounter;
    int tsFreq;
}AI;

typedef struct{
    objectType_t type;
    SDL_Rect rect;
    int obj_id;
    char name[30];

    //Transform
    double rotation;
    bool solid;

    //AI
    AI ai;

    //Item
    ItemInfo itemInfo;

    //Projectiles
    BulletInfo bulletInfo;

    //Player
    PlayerInfo playerInfo;
    
    Connections connetions;

}GameObject;

typedef struct{
    GameObject objects[1024];
    int objCount;
    int nextId;
    SDL_mutex *obj_mutex;

} Scene;

typedef struct{
    unsigned char queue[128][128]; //Kö från klienterna till main
    int size;

}msg_stack;

extern Connections client[N_CLIENTS];



#endif /* defined(__SDL_net__server_structs__) */
