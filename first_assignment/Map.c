#include "Map.h"
#include "Graph.h"

//Κόμβος του hash table
struct map_node{
	char* keyId; 
	//δείκτης σε κ΄΄ομβο του γράφου μας GraphNode = struct graph_node*. 
	//Έτσι και έχουμε πολύ γρήγορη πρόσβαση στον γράφο και διατηρούμε την ανεξαρτησία των δύο δομών όπως ζητείται
	GraphNode value;    
};

struct map {
	List* arrayOfBuckets;	    // Ο πίνακας των buckets του hash table(seperate chainening)
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει εώς τώρα.
	//συναρτήσεις που θα χρησιμοποιηθούν από την λίστα στα buckets
	DestroyFunc destroyMapNodes;
	CompareFunc compareMapNodes;
};


//η compare και η destroy για τις Lists απο MapNode
Map mapCreate(CompareFunc compare, DestroyFunc destroy, int sizeByFile) {

	Map map = malloc(sizeof(*map));
	map->capacity = sizeByFile * 3 + 7;
	map->arrayOfBuckets = malloc(sizeof(List) * map->capacity);
	map->destroyMapNodes = destroy;
	map->compareMapNodes = compare;

	for (int i = 0; i < map->capacity; i++){
		map->arrayOfBuckets[i] = NULL;
	}
	return map;
}


void mapInsert(Map map, char* key, Pointer value) {
	int pos = hashFunction(key) % map->capacity; // mod για να μην ξεπεράσουμε το μέγεθος του πίνακα
	
	//αν είναι ο πρώτος κόμβος που hasharei σε αυτήν την θέση του πίνακα
	if(map->arrayOfBuckets[pos] == NULL){
		map->arrayOfBuckets[pos] = listCreate(map->destroyMapNodes, map->compareMapNodes);
	}

	MapNode node = malloc(sizeof(*node));
	node->value = value;
	node->keyId = key;

	//Η Λιστα έχει MapNode
	listInsert((map->arrayOfBuckets[pos]), node);
}


void mapRemove(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if (node == NULL){
		return;
	}
	else{
		listDeleteNode(map->arrayOfBuckets[hashFunction(key) % map->capacity], node);
	}
}

Pointer mapFind(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if(node == NULL){
		return NULL;
	}
	else{
		return node->value;
	}
}

//επιστρέφει τον κόμβο που αντιστοιχεί στο key , καλείται από την mapFind
MapNode mapFindNode(Map map, char* key) {
	int pos = hashFunction(key) % map->capacity;
	
	if( map->arrayOfBuckets[pos] == NULL){
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
void mapDestroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if(map->arrayOfBuckets[i] != NULL){
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

// djb2 hash function για strings (όπως αναφέρθηκε στο piazza επιτρέπεται να χρησιμοποιηθεί έτοιμη συνάρτηση)
unsigned int hashFunction(char* value) {
    unsigned int  hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;
    return hash;
}