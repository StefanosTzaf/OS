// Υλοποιούμε ένα Map μέσω Hash Table με seperate chaining
#include "hash.h"
#include <stdio.h>

//η compare και η destroy για τις Lists απο MapNode
Map mapCreate(CompareFunc compare, DestroyFunc destroy) {

	Map map = malloc(sizeof(*map));
	map->capacity = 1543;
	map->size = 0;
	map->arrayOfBuckets = malloc(sizeof(List) * map->capacity);

	for (int i = 0; i < map->capacity; i++){
		map->arrayOfBuckets[i] = listCreate(destroy, compare);
	}

	return map;
}

void mapInsert(Map map, char* key, Pointer value) {
	int pos = hashFunction(key)%map->capacity;
	//Δημιουργία Mapnode
	MapNode node = malloc(sizeof(*node));
	node->keyId = malloc(strlen(key) + 1);
	strcpy(node->keyId, key);
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
		free(node->keyId);
		//η μνήμη απελευθερώνετι απο το Graph
		node->value = NULL;
	}
	map->size--;

	return true;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer mapFindNode(Map map, char* key) {

	int pos = hashFunction(key) % map->capacity;
	if(map->arrayOfBuckets[pos]->size == 0){
		return NULL;
	}

	//Η δουλεία της find θα γίνει ουσιαστικά από την compare που παραθέτουμε κάθε φορά στην λίστα
	ListNode node = listFind(map->arrayOfBuckets[pos], key);
	if(node == NULL){
		return NULL;
	}
	else{
		return ((MapNode)node->value)->value;
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


Pointer map_node_key(Map map, MapNode node) {
	return node->keyId;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}


// djb2 hash function για συμβολοσειρές όπως αναφέρθηκε στο piazza μπορούμε να χρησιμοποιήσουμε έτοιμη Hash function
unsigned int hashFunction(char* value) {
    unsigned int  hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}

