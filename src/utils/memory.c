#include "memory.h"
#include <stdlib.h>
#include <string.h>

// Create a new entity list with dynamic allocation
EntityList* createEntityList(int capacity) {
    if (capacity <= 0) {
        return NULL;
    }
    
    EntityList* list = malloc(sizeof(EntityList));
    if (!list) {
        return NULL;
    }
    
    list->entities = malloc(sizeof(Entity) * capacity);
    if (!list->entities) {
        free(list);
        return NULL;
    }
    
    list->size = 0;
    list->capacity = capacity;
    
    return list;
}

// Destroy an entity list and free memory
void destroyEntityList(EntityList* list) {
    if (list) {
        if (list->entities) {
            free(list->entities);
        }
        free(list);
    }
}

// Initialize an entity list with existing array (no allocation)
void initEntityList(EntityList* list, Entity* entityArray, int capacity) {
    if (!list || !entityArray || capacity <= 0) {
        return;
    }
    
    list->entities = entityArray;
    list->size = 0;
    list->capacity = capacity;
}

void pushBack(EntityList* entityList, Entity entity) {
    if (!entityList || !entityList->entities) {
        return;
    }
    
    // Check if there's space in the list
    if (entityList->size < entityList->capacity) {
        entityList->entities[entityList->size] = entity;
        entityList->size++;
    }
    // Note: If list is full, entity is not added
}

void removeEntity(EntityList* entityList, Entity* entity) {
    if (!entityList || !entityList->entities || !entity || entityList->size == 0) {
        return;
    }
    
    // Find the entity in the array by comparing pointers
    for (int i = 0; i < entityList->size; i++) {
        if (&entityList->entities[i] == entity) {
            // Shift all entities after this one to the left
            for (int j = i; j < entityList->size - 1; j++) {
                entityList->entities[j] = entityList->entities[j + 1];
            }
            entityList->size--;
            break;
        }
    }
}

void removeEntityAtIndex(EntityList* entityList, int index) {
    if (!entityList || !entityList->entities || index < 0 || index >= entityList->size) {
        return;
    }
    
    // Shift all entities after this index to the left
    for (int i = index; i < entityList->size - 1; i++) {
        entityList->entities[i] = entityList->entities[i + 1];
    }
    entityList->size--;
}

int getEntityListSize(EntityList* list) {
    if (!list) {
        return 0;
    }
    return list->size;
}

Entity* getEntityAtIndex(EntityList* list, int index) {
    if (!list || !list->entities || index < 0 || index >= list->size) {
        return NULL;
    }
    return &list->entities[index];
}

void clearEntityList(EntityList* list) {
    if (list) {
        list->size = 0;
    }
}

int isEntityListFull(EntityList* list) {
    if (!list) {
        return 1; // Consider NULL list as "full"
    }
    return list->size >= list->capacity;
}

int isEntityListEmpty(EntityList* list) {
    if (!list) {
        return 1;
    }
    return list->size == 0;
}
