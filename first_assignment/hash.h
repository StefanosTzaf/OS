#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


typedef struct map* Map;
typedef struct map_node* MapNode;
typedef void* Pointer;

//όπως και στην υλοποίηση της λίστας χρησιμοποιούμε δεικτες σε συναρτήσεις ώστε η compare να λειτουργεί βάση αυτων που όντως θέλουμε να συγκρίνουμε(περιεχόμενο pointers)
//η destroy_func να απελευθερώνει όλη την μνήμη αποδοτικά
typedef void (*DestroyFunc)(Pointer value);
typedef int (*CompareFunc)(Pointer a, Pointer b);

// Δημιουργεί και επιστρέφει ένα map, στο οποίο τα στοιχεία συγκρίνονται με βάση
// τη συνάρτηση compare.
// Αν destroyKey ή/και destroyValue != NULL, τότε καλείται destroyKey(key)
// ή/και destroyValue(value) κάθε φορά που αφαιρείται ένα στοιχείο.

int compareId(Pointer a, Pointer b);
Map mapCreate();

// Επιστρέφει τον αριθμό στοιχείων που περιέχει το map.

int map_size(Map map);

// Προσθέτει το κλειδί key με τιμή value. Αν υπάρχει κλειδί ισοδύναμο με key, τα παλιά key & value αντικαθίσταται από τα νέα.
//
// ΠΡΟΣΟΧΗ:
// Όσο το key είναι μέλος του map, οποιαδήποτε μεταβολή στο περιεχόμενό του (στη μνήμη που δείχνει) έχει μη ορισμένη συμπεριφορά.

void mapInsert(Map map, char* key, Pointer value);

// Αφαιρεί το κλειδί που είναι ισοδύναμο με key από το map, αν υπάρχει.
// Επιστρέφει true αν βρέθηκε τέτοιο κλειδί, διαφορετικά false.
bool mapRemove(Map map, char* key);

// Επιστρέφει την τιμή που έχει αντιστοιχιστεί στο συγκεκριμένο key, ή NULL αν το key δεν υπάρχει στο map.
//
// Προσοχή: η συνάρτηση επιστρέφει NULL είτε όταν το key δεν υπάρχει, είτε όταν υπάρχει και έχει τιμή NULL.
//          Αν χρειάζεται να διαχωρίσουμε τις δύο περιπτώσεις μπορούμε να χρησιμοποιήσουμε την mapFindNode.

Pointer mapFind(Map map, char* key);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το map.
// Οποιαδήποτε λειτουργία πάνω στο map μετά το destroy είναι μη ορισμένη.

void map_destroy(Map map);

// Διάσχιση του map μέσω κόμβων ////////////////////////////////////////////////////////////
//
// Η σειρά διάσχισης είναι αυθαίρετη.

// Η σταθερά αυτή συμβολίζει έναν εικονικό κόμβου _μετά_ τον τελευταίο κόμβο του map
#define MAP_EOF (MapNode)0

// Επιστρέφει το κλειδί του κόμβου node

Pointer map_node_key(Map map, MapNode node);

// Επιστρέφει το περιεχόμενο του κόμβου node

Pointer map_node_value(Map map, MapNode node);

// Βρίσκει και επιστρέφεο τον κόμβο που έχει αντιστοιχιστεί στο κλειδί key,
// ή MAP_EOF αν το κλειδί δεν υπάρχει στο map.

MapNode mapFindNode(Map map, char* key);

unsigned int hashFunction(char* value);		// Χρήση όταν το key είναι char*
