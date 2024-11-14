#include "Map.h"

// Node of the hash table
struct map_node {
	char* keyId; 
	Pointer value;    
};

struct map {
	List* arrayOfBuckets;	    // The array of buckets of the hash table (separate chaining)
	int capacity;				// How much space we have allocated so far.
	// Functions that will be used by the list in the buckets
	DestroyFunc destroyMapNodes;
	CompareFunc compareMapNodes;
};

// The compare and destroy functions for the Lists of MapNode
Map mapCreate(CompareFunc compare, DestroyFunc destroy, int sizeByFile) {

	Map map = malloc(sizeof(*map));
	map->capacity = sizeByFile * 3 + 7;
	map->arrayOfBuckets = malloc(sizeof(List) * map->capacity);
	map->destroyMapNodes = destroy;
	map->compareMapNodes = compare;

	for (int i = 0; i < map->capacity; i++) {
		map->arrayOfBuckets[i] = NULL;
	}
	return map;
}

void mapInsert(Map map, char* key, Pointer value) {
	int pos = hashFunction(key) % map->capacity; // mod to not exceed the size of the array
	
	// if it is the first node that hashes to this position of the array
	if (map->arrayOfBuckets[pos] == NULL) {
		map->arrayOfBuckets[pos] = listCreate(map->destroyMapNodes, map->compareMapNodes);
	}

	MapNode node = malloc(sizeof(*node));
	node->value = value;
	node->keyId = key;

	// The list contains MapNode
	listInsert((map->arrayOfBuckets[pos]), node);
}

void mapRemove(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if (node == NULL) {
		return;
	} 
	else{	
		listDeleteNode(map->arrayOfBuckets[hashFunction(key) % map->capacity], node);
	}
}

Pointer mapFind(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if (node == NULL) {
		return NULL;
	} 
	else{
		return node->value;
	}
}

// Returns the node that corresponds to the key, called by mapFind
MapNode mapFindNode(Map map, char* key) {
	int pos = hashFunction(key) % map->capacity;
	
	if (map->arrayOfBuckets[pos] == NULL) {
		return NULL;
	}
	// The work of find will essentially be done by the compare function that we provide each time to the list
	ListNode node = listFind(map->arrayOfBuckets[pos], key);
	if (node == NULL) {
		return NULL;
	} 
	else{
		return listNodeValue(node);
	}
}

void mapDestroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->arrayOfBuckets[i] != NULL) {
			listDestroy(map->arrayOfBuckets[i]);
		}
	}
	free(map->arrayOfBuckets);
	free(map);
}

Pointer mapNodeKey(Map map, MapNode node) {
	return node->keyId;
}

Pointer mapNodeValue(MapNode node) {
	return node->value;
}

// djb2 hash function for strings (as mentioned on Piazza, it is allowed to use a ready-made function)
unsigned int hashFunction(char* value) {
	unsigned int hash = 5381;
	for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;
	return hash;
}
