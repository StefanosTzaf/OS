// Υλοποιούμε ένα Map μέσω Hash Table με seperate chaining
#pragma once  //οδηγεία προς επεξεργαστή να μην αφήσει το ενα header να συμπεριληφθεί παραπάνω από μια φορά
#include <stdbool.h>
#include <string.h>
#include "List.h"

typedef struct map* Map;
typedef struct map_node* MapNode;

//Ο(n) αρχικοποίηση των λιστών του seperate chaining με NULL 
Map mapCreate(CompareFunc compare, DestroyFunc destroy, int sizeByFile);

//Ο(1)
void mapInsert(Map map, char* key, Pointer value);

//O(1)
void mapRemove(Map map, char* key);

// Επιστρέφει την τιμή που έχει αντιστοιχιστεί στο συγκεκριμένο key, ή NULL αν το key δεν υπάρχει στο map.O(1)
Pointer mapFind(Map map, char* key);
MapNode mapFindNode(Map map, char* key);

//O(n)
void mapDestroy(Map map);

//O(1)
Pointer mapNodeKey(Map map, MapNode node);
Pointer mapNodeValue(MapNode node);

unsigned int hashFunction(char* value);