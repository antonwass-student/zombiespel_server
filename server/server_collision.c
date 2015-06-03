#include "server_collision.h"
#include "server_structs.h"
#include "spel_objects.h"
#include "net_msgs.h"

bool MoveObject(GameObject* movingObject, Scene* scene, int speedX, int speedY, int objectIndex)
{
    bool colUp = false, colDown = false, colLeft = false, colRight = false;
    movingObject->rect.x += speedX;
    movingObject->rect.y += speedY;
    
    for(int i = 0; i < scene->objCount; i++) // Kollar kollision mellan alla objekt i scene
    {
        if(scene->objects[i].obj_id == movingObject->obj_id)
            continue;
        
        if(movingObject->rect.x <= scene->objects[i].rect.x + scene->objects[i].rect.w &&
           movingObject->rect.x >= scene->objects[i].rect.x) // kollision vänster av objekt
        {
            colLeft = true;
        }
        else if(movingObject->rect.x + movingObject->rect.w  >= scene->objects[i].rect.x &&
                movingObject->rect.x + movingObject->rect.w  <= scene->objects[i].rect.x + scene->objects[i].rect.w) // kollision höger av objekt
        {
            colRight = true;
        }
        
        if(movingObject->rect.y <= scene->objects[i].rect.y + scene->objects[i].rect.h &&
           movingObject->rect.y >= scene->objects[i].rect.y) // kollision ovanifrån objekt
        {
            colUp = true;
        }
        else if(movingObject->rect.y + movingObject->rect.h >= scene->objects[i].rect.y &&
                movingObject->rect.y + movingObject->rect.h <= scene->objects[i].rect.y + scene->objects[i].rect.h) // kollision underifrån objekt
        {
            colDown = true;
        }
        
        if((colLeft || colRight) && (colUp || colDown)) // Collision between objects!
        {
            CollisionHandler(movingObject, &scene->objects[i], objectIndex, i, scene);
        }
        
        if(movingObject->solid && scene->objects[i].solid) //Rättar till positionen om en kollision uppsttått
        {
            if(colLeft && (colUp || colDown))
            {
                movingObject->rect.x -= speedX;
                
            }
            else if(colRight && (colUp || colDown))
            {
                movingObject->rect.x -= speedX;
            }
            
            if(colUp && (colLeft || colRight))
            {
                movingObject->rect.y -= speedY;
            }
            else if(colDown && (colLeft || colRight))
            {
                movingObject->rect.y -= speedY;
            }
        }
        
        ProximityCheck(movingObject, &scene->objects[i], objectIndex, i, scene);
        
        colLeft = false;
        colRight = false;
        colUp = false;
        colDown = false;
    }
    return true;
}

void ProximityCheck(GameObject* obj1, GameObject* obj2, int obj1_index,int obj2_index, Scene* scene)
{
    int distance = GetDistance(obj1->rect, obj2->rect);
    
    if(obj1->type == OBJECT_PLAYER && obj2->type == OBJECT_ITEM) //player with item
    {
        if(obj2->itemInfo.type == ITEM_MEDKIT && distance < 100)
        {
            printf("Players old health = %d\n", obj1->playerInfo.health);
            printf("medkit healed: %d\n", obj2->itemInfo.amount);
            obj1->playerInfo.health += obj2->itemInfo.amount;
            SendPlayerHealth(obj1->obj_id, obj1->playerInfo.health);
            printf("Players new health = %d\n", obj1->playerInfo.health);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_WEAPON_1 && distance < 100)
        {
            printf("Collided with machine gun\n");
            SendWeapon(obj1->obj_id, obj2->itemInfo.type);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_ARMOR && distance < 100)
        {
            obj1->playerInfo.armor += obj2->itemInfo.amount;
            SendArmor(obj1->obj_id, obj2->itemInfo.amount);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_AMMO && distance < 100)
        {
            obj1->playerInfo.ammoTotal += obj2->itemInfo.amount;
            SendAmmo(obj1->obj_id, obj2->itemInfo.amount);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_WEAPON_2 && distance < 100)
        {
            SendWeapon(obj1->obj_id, obj2->itemInfo.type);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_WEAPON_3 && distance < 100)
        {
            SendWeapon(obj1->obj_id, obj2->itemInfo.type);
            RemoveObject(scene, obj2->obj_id);
        }
    }
}

void CollisionHandler(GameObject* collider1, GameObject* collider2, int c1_index, int c2_index, Scene* scene)
{
    if(collider1->type == OBJECT_BULLET)//Collision with object1
    {
        if(collider1->bulletInfo.type == BULLET_PLAYER && (collider2->type == OBJECT_NPC || collider2->type ==OBJECT_ZOMBIE_SPITTER))//Bullet with zombie
        {
            printf("Bullet(%d) hit a zombie\n", collider1->obj_id);
            collider2->ai.health -= collider1->bulletInfo.damage;
            
            if(collider2->ai.health <= 0)
            {
                GameObject obj;
                printf("Bullet(%d) being removed.\n", collider1->obj_id);
                RemoveObject(scene, collider1->obj_id);
                obj = CreateMedkit(collider2->rect.x, collider2->rect.y, scene->nextId++);
                printf("Zombie(%d) dropped a medkit(%d)\n",collider2->obj_id, obj.obj_id);
                AddObject(scene, obj, true);
                RemoveObject(scene, collider2->obj_id);
                return;
            }
            else
            {
                printf("Bullet(%d) being removed.\n", collider1->obj_id);
                RemoveObject(scene, collider1->obj_id);
            }
        }
        else if(collider1->bulletInfo.type == BULLET_ZOMBIE && collider2->type == OBJECT_PLAYER)
        {
            collider2->playerInfo.health -= collider1->bulletInfo.damage;
            SendPlayerHealth(collider2->obj_id, collider2->playerInfo.health);
            RemoveObject(scene, collider1->obj_id);
        }
    }
    else if(collider1->type == OBJECT_BULLET && collider2->type == OBJECT_WALL)
    {
        printf("Bullet collided with Wall\n");
        RemoveObject(scene, c1_index);
    }
    else if(collider1->type == OBJECT_BULLET && collider2->type == OBJECT_CAR)
    {
        printf("Bullet collided with Car\n");
        RemoveObject(scene, c1_index);
    }
}


int GetDistance(SDL_Rect obj1, SDL_Rect obj2)
{
    return (int)(sqrt(pow((obj1.x + (obj1.w/2)) - (obj2.x + (obj2.w/2)), 2) + pow((obj1.y + (obj1.h/2)) - (obj2.y + (obj2.h/2)), 2)));
}

int NewDamage(GameObject* NPC, GameObject* Player)
{
    int NewDamage=0;
    
    if(NPC->type == OBJECT_NPC)
    {
        NewDamage= ((100 - Player->playerInfo.armor)/100.0f) * NPC->ai.damage;
        return NewDamage;
    }
    else if(NPC->type == OBJECT_BULLET && NPC->bulletInfo.type == BULLET_ZOMBIE){
        NewDamage= ((100 - Player->playerInfo.armor)/100.0f) * NPC->bulletInfo.damage;
        return NewDamage;
    }
    return NewDamage;
}