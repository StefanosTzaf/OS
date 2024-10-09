#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "List.h"

typedef struct map* Map;
typedef struct map_node* MapNode;
typedef void* Pointer;



Map mapCreate(CompareFunc compare, DestroyFunc destroy, int sizeByFile);


void mapInsert(Map map, char* key, Pointer value);


bool mapRemove(Map map, char* key);

// Επιστρέφει την τιμή που έχει αντιστοιχιστεί στο συγκεκριμένο key, ή NULL αν το key δεν υπάρχει στο map.

Pointer mapFind(Map map, char* key);

MapNode mapFindNode(Map map, char* key);

int sizeOfMap(Map map);

void map_destroy(Map map);


Pointer mapNodeKey(Map map, MapNode node);

Pointer mapNodeValue(MapNode node);

unsigned int hashFunction(char* value);		// Χρήση όταν το key είναι char*