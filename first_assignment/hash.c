// Υλοποιούμε ένα Map μέσω Hash Table με open addressing (linear probing)
#include "hash.h"
#include "Graph.h"
#include <stdio.h>


//Κόμβος του hash table
struct map_node{
	char* keyId;        //του κάθε χρήστη
	GraphNode value;    //δείκτης σε κ΄΄ομβο του γράφου μας GraphNode = struct graph_node*. Έτσι και έχουμε πολύ γρήγορη πρόσβαση στον γράφο και διατηρούμε την ανεξαρτησία των δύο δομών όπως ζητείται
};

struct map {
	List* arrayOfBuckets;	// Ο πίνακας των buckets του hash table(seperate chainening)
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει εώς τώρα.
	int size;					// Πόσα στοιχεία έχει το hash table
};


//η compare και η destroy για τις Lists απο MapNode
Map mapCreate(CompareFunc compare, DestroyFunc destroy, int sizeByFile) {

	Map map = malloc(sizeof(*map));
	map->capacity = sizeByFile * 2 + 15;
	map->size = 0;
	map->arrayOfBuckets = malloc(sizeof(List) * map->capacity);

	for (int i = 0; i < map->capacity; i++){
		map->arrayOfBuckets[i] = listCreate(destroy, compare);
	}

	return map;
}




void mapInsert(Map map, char* key, Pointer value) {
	int pos = hashFunction(key) % map->capacity;
	//Δημιουργία Mapnode
	MapNode node = malloc(sizeof(*node));
	node->value = value;
	//Η Λιστα έχει MapNode
	listInsert((map->arrayOfBuckets[pos]), node);
	// Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map
	map->size++;
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool mapRemove(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if (node == NULL){
		return false;
	}
	else{
		listDeleteNode(map->arrayOfBuckets[hashFunction(key) % map->capacity], node);
	}
	map->size--;

	return true;
}


// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer mapFind(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if(node == NULL){
		return NULL;
	}
	else{
		return node->value;
	}
}


MapNode mapFindNode(Map map, char* key) {
	int pos = hashFunction(key) % map->capacity;
	
	if(listSize( map->arrayOfBuckets[pos] )== 0){
		return NULL;
	}
	//Η δουλεία της find θα γίνει ουσιαστικά από την compare που παραθέτουμε κάθε φορά στην λίστα
	ListNode node = listFind(map->arrayOfBuckets[pos], key);
	if(node == NULL){
		return NULL;
	}
	else{
		return listNodeValue(node);
	}
}

//κάνουμε μόνο τους δείκτες NULL γιατί η destroyGraph έχει αποδεσμέυσει την μνήμη των κόμβων

void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		listDestroy(map->arrayOfBuckets[i]);
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
int sizeOfMap(Map map){
	return map->size;
}

// djb2 hash function
unsigned int hashFunction(char* value) {
    unsigned int  hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}


