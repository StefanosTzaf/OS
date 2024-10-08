// Υλοποιούμε ένα Map μέσω Hash Table με open addressing (linear probing)
#include "hash.h"
#include "Graph.h"
#include <stdio.h>
//ο load factor στο linear probing πρέπει να είναι το πολύ 0.5
#define MAX_LOAD_FACTOR 0.5


//οι κόμβοι μπορεί να ειναι σε 3 διαφορετικές καταστάσεις: αν διαγραφεί ενας κόμβος μπορούμε να κάνουμε insert σε αυτήν την θέση
//αλλά στην αναζήτηση πρέπει να προσπεράσουμε τον κόμβο αυτόν
typedef enum {
	EMPTY, OCCUPIED, DELETED
} StateOfNode;


//Από θεωρία δομών δεδομένων το μέγεθος του Hash Table θέλουμε να είναι πρώτος αριθμός.
//Αν κάνουμε rehash ακολόυθούμε αυτήν την λίστα.
int sizesOfPrimes[] = {53, 97, 193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317, 196613, 393241,
	786433, 1572869, 3145739, 6291469, 12582917, 25165843, 50331653, 100663319, 201326611, 402653189, 805306457, 1610612741};



//Κόμβος του hash table
struct map_node{
	char* keyId;        //του κάθε χρήστη
	GraphNode value;    //δείκτης σε κ΄΄ομβο του γράφου μας GraphNode = struct graph_node*. Έτσι και έχουμε πολύ γρήγορη πρόσβαση στον γράφο και διατηρούμε την ανεξαρτησία των δύο δομών όπως ζητείται
	StateOfNode state;
};

struct map {
	MapNode array;
	int capacity;				// Πόσο χώρο έχουμε δεσμεύσει εώς τώρα.
	int size;					// Πόσα στοιχεία έχει το hash table
    CompareFunc compare;
};

int compareId(Pointer a, Pointer b){
    return strcmp((char*)a, (char*)b);
}

Map mapCreate() {

	Map map = malloc(sizeof(*map));
	map->capacity = sizesOfPrimes[0];
	map->array = malloc(sizeof(struct map_node) * map->capacity);

	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;

	map->size = 0;
	map->compare = compareId;   //η συγγκριση γίνεται βάση του id του κόμβου

	return map;
}





static void rehash(Map map) {
	// Αποθήκευση των παλιών δεδομένων
	int oldCapacity = map->capacity;
	MapNode oldArray = map->array;

	// Βρίσκουμε τη νέα χωρητικότητα, διασχίζοντας τη λίστα των πρώτων ώστε να βρούμε τον επόμενο. 
	int countOfPrimes = sizeof(sizesOfPrimes) / sizeof(int);// το μέγεθος του πίνακα
	for (int i = 0; i < countOfPrimes; i++) {
		if (sizesOfPrimes[i] > oldCapacity) {
			map->capacity = sizesOfPrimes[i]; 
			break;
		}
	}

	//Aν δεν τπάρχει μεγαλύτερος πρώτος αριθμός στον sizesOfPrimes από τον τωρινό τότε διπλασιάζουμε το μέγεθος του πίνακα
	if (map->capacity == oldCapacity)
		map->capacity *= 2;

    //δέσμεσυση χώρου για το νεο hash table
	map->array = malloc(map->capacity * sizeof(struct map_node));
	for (int i = 0; i < map->capacity; i++)
		map->array[i].state = EMPTY;


	map->size = 0;
	for (int i = 0; i < oldCapacity; i++)
		if (oldArray[i].state == OCCUPIED)
			mapInsert(map, oldArray[i].keyId, oldArray[i].value);

	//Αποδεσμεύουμε τον παλιό πίνακα ώστε να μήν έχουμε leaks
	free(oldArray);//απελευθερώνουμε τον παλιό πίνακα απο τους pointers αλλά όχι όλη την μνήμη των pointers αυτών γιαττί θα επηρέαζε καιτ ον γράφο μας
}


void mapInsert(Map map, char* key, Pointer value) {
	// Σκανάρουμε το Hash Table μέχρι να βρούμε διαθέσιμη θέση για να τοποθετήσουμε το ζευγάρι,
	// ή μέχρι να βρούμε το κλειδί ώστε να το αντικαταστήσουμε.
	MapNode node = NULL;
	unsigned int pos;
    //θέλουμε να κάνουμε hash βάση του id του κόμβου αφού θα ψάχνουμε βάση του id. Ομως να ΄αποθηκεύουμε δείkτη σε κόμβο δηάδη GraphNode

	for (pos = hashFunction(key) % map->capacity;		// ξεκινώντας από τη θέση που κάνει hash το key
		map->array[pos].state != EMPTY;						// αν φτάσουμε σε EMPTY σταματάμε
		pos = (pos + 1) % map->capacity) {					// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα

		if (map->array[pos].state == DELETED) {
			// Βρήκαμε DELETED θέση. Θα μπορούσαμε να βάλουμε το ζευγάρι εδώ, αλλά _μόνο_ αν το key δεν υπάρχει ήδη.
			// Οπότε σημειώνουμε τη θέση, αλλά συνεχίζουμε την αναζήτηση, το key μπορεί να βρίσκεται πιο μετά.
			if (node == NULL)
				node = &map->array[pos];

		} 
		//Aν βρούμε το key, το ζευγάρι θα μπει αναγκαστικά εδώ.
		else if (map->compare(map->array[pos].keyId, key) == 0) {
			node = &map->array[pos];						
			break;
		}
	}
	// αν βρήκαμε EMPTY (όχι DELETED, ούτε το key), το node δεν έχει ακόμα τιμή
	if (node == NULL)
		node = &map->array[pos];

	// Προσθήκη τιμών στον κόμβο
	node->state = OCCUPIED;
	node->keyId = key;
	node->value = value;

	// Νέο στοιχείο, αυξάνουμε τα συνολικά στοιχεία του map
	map->size++;
	
	// Αν με την νέα εισαγωγή ξεπερνάμε το μέγιστο load factor, πρέπει να κάνουμε rehash
	float load_factor = (float)map->size / map->capacity;
	if (load_factor > MAX_LOAD_FACTOR)
		rehash(map);
}

// Διαργραφή απο το Hash Table του κλειδιού με τιμή key
bool mapRemove(Map map, char* key) {
	MapNode node = mapFindNode(map, key);
	if (node == NULL){
		return false;
	}
	else{
		free(node->keyId);
		free(node->value);
	}
	// θέτουμε ως "deleted", ώστε να μην διακόπτεται η αναζήτηση, αλλά ταυτόχρονα να γίνεται ομαλά η εισαγωγή
	node->state = DELETED;
	map->size--;

	return true;
}

// Αναζήτηση στο map, με σκοπό να επιστραφεί το value του κλειδιού που περνάμε σαν όρισμα.

Pointer mapFind(Map map, char* key) {
	MapNode node = mapFindNode(map, key);

	if (node != NULL){
		return node->value;
	}
	else
		return NULL;
}


MapNode mapFindNode(Map map, char* key) {
	// Διασχίζουμε τον πίνακα, ξεκινώντας από τη θέση που κάνει hash το key, και για όσο δε βρίσκουμε EMPTY
	int count = 0;
	for (unsigned int pos = hashFunction(key) % map->capacity;		// ξεκινώντας από τη θέση που κάνει hash το key
		map->array[pos].state != EMPTY;							// αν φτάσουμε σε EMPTY σταματάμε
		pos = (pos + 1) % map->capacity) {						// linear probing, γυρνώντας στην αρχή όταν φτάσουμε στη τέλος του πίνακα

		// Μόνο σε OCCUPIED θέσεις (όχι DELETED), ελέγχουμε αν το key είναι εδώ
		if (map->array[pos].state == OCCUPIED && map->compare(map->array[pos].keyId, key) == 0){
			return &map->array[pos];
		}
		// Αν διασχίσουμε ολόκληρο τον πίνακα σταματάμε. Εφόσον ο πίνακας δεν μπορεί να είναι όλος OCCUPIED,
		// αυτό μπορεί να συμβεί μόνο στην ακραία περίπτωση που ο πίνακας έχει γεμίσει DELETED τιμές!
		count++;
		if (count == map->capacity)
			break;
	}
	
	return NULL;
}

//κάνουμε μόνο τους δείκτες NULL γιατί η destroyGraph έχει αποδεσμέυσει την μνήμη των κόμβων
void map_destroy(Map map) {
	for (int i = 0; i < map->capacity; i++) {
		if (map->array[i].state == OCCUPIED) {
			map->array[i].value = NULL;
		}
	}
	free(map->array);
	free(map);
}


Pointer map_node_key(Map map, MapNode node) {
	return node->keyId;
}

Pointer map_node_value(Map map, MapNode node) {
	return node->value;
}


// djb2 hash function
unsigned int hashFunction(char* value) {
    unsigned int  hash = 5381;
    for (char* s = value; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;			// hash = (hash * 33) + *s. Το foo << 5 είναι γρηγορότερη εκδοχή του foo * 32.
    return hash;
}


