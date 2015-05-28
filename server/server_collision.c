#include "server_collision.h"
#include "server_structs.h"
#include "spel_objects.h"
#include "net_msgs.h"

bool MoveObject(GameObject* movingObject, Scene* scene, int speedX, int speedY, int objectIndex){
    bool colUp = false, colDown = false, colLeft = false, colRight = false;
    int i;
    movingObject->rect.x += speedX;
    movingObject->rect.y += speedY;

    for(i = 0; i < scene->objCount; i++) // Kollar kollision mellan alla objekt i scene
    {
        if(scene->objects[i].obj_id == movingObject->obj_id)
            continue;

        //printf("Checking ColLeft\n");
        //printf("MovingObject x=%d, w=%d\n", movingObject->rect.x, movingObject->rect.w);
       // printf("OtherObject  x=%d, w=%d\n", scene->objects[i].rect.x, scene->objects[i].rect.w);
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
            //printf("Collision detected with %s and %s\n", scene->objects[i].name, movingObject->name);
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


    if(obj1->type == OBJECT_PLAYER && obj2->type == OBJECT_ITEM) //player med item
    {
        if(obj2->itemInfo.type == ITEM_MEDKIT && distance < 100) {

            printf("Players old health = %d\n", obj1->playerInfo.health);
            printf("medkit healed: %d\n", obj2->itemInfo.amount);
            obj1->playerInfo.health += obj2->itemInfo.amount;
            SendPlayerHealth(obj1->obj_id, obj1->playerInfo.health);
            printf("Players new health = %d\n", obj1->playerInfo.health);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_WEAPON_1 && distance < 100) {
            printf("Collided with machine gun\n");
            SendWeapon(obj1->obj_id, obj2->itemInfo.type);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_ARMOR && distance < 100) {
        printf("Debug 1\n");
            obj1->playerInfo.armor += obj2->itemInfo.amount;
            SendArmor(obj1->obj_id, obj2->itemInfo.amount);
            RemoveObject(scene, obj2->obj_id);

        }
        else if(obj2->itemInfo.type == ITEM_AMMO && distance < 100) {
        printf("Debug 2\n");
            obj1->playerInfo.ammoTotal += obj2->itemInfo.amount;
            SendAmmo(obj1->obj_id, obj2->itemInfo.amount);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_WEAPON_2 && distance < 100) {
        printf("Debug 3\n");
            SendWeapon(obj1->obj_id, obj2->itemInfo.type);
            RemoveObject(scene, obj2->obj_id);
        }
        else if(obj2->itemInfo.type == ITEM_WEAPON_3 && distance < 100) {
        printf("Debug 4\n");
            SendWeapon(obj1->obj_id, obj2->itemInfo.type);
            RemoveObject(scene, obj2->obj_id);
        }

    }
    /*
    if(obj1->type == OBJECT_PLAYER && obj2->type == OBJECT_NPC){
        if(obj1->type == OBJECT_PLAYER && distance < 85){
            if (obj2->ai.atkTimer == 0)
            {
                //Attack player
                obj1->playerInfo.health -= NewDamage(obj2,obj1);
                obj2->ai.atkTimer = (int)(obj2->ai.atkCd * 60.0f);
                printf("ai.atkTimer = %d\n", obj2->ai.atkTimer);
                if(obj1->playerInfo.health <= 0)
                {
                    printf("Player died!\n");
                    obj1->rect.x = 3000;
                    obj1->rect.y = 5200;
                    obj1->playerInfo.health = 100;
                    obj1->ai.target = NULL;
                }
                printf("Player health is now: %d\n", obj1->playerInfo.health);
            }
        }
    }*/



    /*
    if(obj1->type == OBJECT_NPC&& obj2->type == OBJECT_EXPLOSION){
        if(obj1->type == OBJECT_NPC && distance < 100){
            obj1->ai.health -= obj2->ExplosionInfo.damage;
            if(obj1->ai.health <= 0){
                printf("NPC died %s! from explosion\nIndex:%d\n",obj1->name,obj1_index);
                RemoveObjectFromScene(scene, obj1_index);
            }
        }
    }*/
}

void CollisionHandler(GameObject* collider1, GameObject* collider2, int c1_index, int c2_index, Scene* scene)
{

    //printf("CollisionHandler with object1:%d and object2:%d\n",collider1->obj_id,collider2->obj_id);
    if(collider1->type == OBJECT_BULLET)
    {
        if(collider1->bulletInfo.type == BULLET_PLAYER && (collider2->type == OBJECT_NPC || collider2->type == OBJECT_ZOMBIE_SPITTER))//Bullet med zombie
        {
            printf("Bullet(%d) hit a zombie\n", collider1->obj_id);
            //printf("Bullet collided with NPC\n");
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


                //  TODO:::Skapa någon slags drop-funktion.
                //createObject(scene, OBJECT_ITEM, "MedKit", collider2->rect.x, collider2->rect.y, 50, 50, TXT_MEDKIT, false); //Lägg till create medkit
            }
            else
            {
            printf("Bullet(%d) being removed.\n", collider1->obj_id);
            RemoveObject(scene, collider1->obj_id);
            }
        }
        else if(collider1->bulletInfo.type == BULLET_ZOMBIE && collider2->type == OBJECT_PLAYER)
        {
                //TODO: Skada spelare

                collider2->playerInfo.health -= collider1->bulletInfo.damage;
                SendPlayerHealth(collider2->obj_id, collider2->playerInfo.health);
                RemoveObject(scene, collider1->obj_id);
        }

    }
    else if(collider1->type == OBJECT_BULLET &&  collider2->type == OBJECT_PLAYER)
    {/*
        if (collider1->bulletInfo.type == BULLET_ZOMBIE)
        {
            collider2->playerInfo.health -= NewDamage(collider1,collider2);
            if(collider2->playerInfo.health <= 0)
            {

                // TODO:: Skapa en respawn funktion. (game over funktion)
                printf("Player died!\n");
                collider2->rect.x = 3000;
                collider2->rect.y = 5200;
                collider2->playerInfo.health = 100;
                collider2->ai.target = NULL;
            }
            RemoveObject(scene, collider1->obj_id);

        }*/

    }

    else if(collider1->type == OBJECT_BULLET && collider2->type == OBJECT_WALL) //Bullet med Wall
    {
        printf("Bullet collided with Wall\n");
        RemoveObject(scene, c1_index);
    }
    else if(collider1->type == OBJECT_BULLET && collider2->type == OBJECT_CAR) //Bullet med car
    {
        printf("Bullet collided with Car\n");
        RemoveObject(scene, c1_index);
    }
}

int GetDistance(SDL_Rect obj1, SDL_Rect obj2)
{
    return (int)(sqrt(pow((obj1.x + (obj1.w/2)) - (obj2.x + (obj2.w/2)), 2) + pow((obj1.y + (obj1.h/2)) - (obj2.y + (obj2.h/2)), 2)));
}

int NewDamage(GameObject* NPC, GameObject* Player){
    int NewDamage=0;
    if(NPC->type == OBJECT_NPC){
        printf("hej\n");
        NewDamage= ((100 - Player->playerInfo.armor)/100.0f) * NPC->ai.damage;
        return NewDamage;
    }
    else if(NPC->type == OBJECT_BULLET && NPC->bulletInfo.type == BULLET_ZOMBIE){
        NewDamage= ((100 - Player->playerInfo.armor)/100.0f) * NPC->bulletInfo.damage;
        return NewDamage;
    }
    return NewDamage;
}

