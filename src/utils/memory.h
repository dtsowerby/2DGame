#pragma once

#include "entity.h"

// Entity list structure to manage multiple lists
typedef struct EntityList {
    Entity* entities;
    int size;
    int capacity;
} EntityList;

// Entity list management functions
EntityList* createEntityList(int capacity);
void destroyEntityList(EntityList* list);
void initEntityList(EntityList* list, Entity* entityArray, int capacity);

// Array operations
void pushBack(EntityList* entityList, Entity entity);
void removeEntity(EntityList* entityList, Entity* entity);
void removeEntityAtIndex(EntityList* entityList, int index);

// Utility functions
int getEntityListSize(EntityList* list);
Entity* getEntityAtIndex(EntityList* list, int index);
void clearEntityList(EntityList* list);
int isEntityListFull(EntityList* list);
int isEntityListEmpty(EntityList* list);
