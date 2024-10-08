#include "List.h"

//διπλα συνδεδεμενη λιστα θα την χρειαστουμε για την διαγραφη
//Η λιστα θα είναι ένας pointer σε αυτό το struct
struct list {
	ListNode head;
	ListNode last;
	int size;
	DestroyFunc destroyValue;
	CompareFunc compare;
};

struct list_node {
	ListNode next;
	ListNode prev;
	Pointer value;
};


List listCreate(DestroyFunc destroyValue, CompareFunc compare) {
	// Πρώτα δημιουργούμε το stuct
	List list = malloc(sizeof(*list));
	list->size = 0;
	list->last = NULL;
	list->head = NULL;
	list->destroyValue = destroyValue;
	list->compare = compare;
	return list;
}

void listInsert(List list, Pointer value) {
	ListNode new = malloc(sizeof(*new));
	new->value = value;
	new->next = NULL;	//προσθέτουμε τον κόμβο στο τέλος της λίστας
	new->prev = list->last;	
	//Εαν υπάρχει κόμβος μέσα στην λίστα
	if (list->last != NULL){
		list->last->next = new;
	}
	else{//αν η λιστα είναι κενή
		list->head = new;
	}
	list->last = new;
	list->size++;
}


void listDeleteNode(List list, Pointer value){
    // Έλεγχος αν η λίστα είναι άδεια

	ListNode node = listFind(list, value);
	
    if (node == NULL) {
	    return;
    }

	ListNode previousNode = node->prev;
	ListNode nextNode = node->next;

	if (previousNode == NULL && nextNode == NULL) {
		list->head = NULL;
		list->last = NULL;
	}
	else if (previousNode == NULL) {//ο κόμβος ήταν στην αρχή αλλά έχει επόμενο
		list->head = nextNode;
		nextNode->prev = NULL;
	}
	else if (nextNode == NULL) {//ο κόμβος ήταν στο τέλος αλλά έχει προηγούμενο
		list->last = previousNode;
		previousNode->next = NULL;
	}
	else {
		previousNode->next = nextNode;
		nextNode->prev = previousNode;
	}
	// Απελευθέρωση μνήμης του κόμβου που διαγράφηκε
	list->destroyValue(node->value);
	free(node);
	list->size--;

}


void listDestroy(List list) {
	ListNode node = list->head;
	while (node != NULL) {
		ListNode next = node->next;
		//κανουμε destroy τον κάθε κόμβπ βάση της συνάρτησης destroyValue
		//(o Pointer μπορει να δειχνει σε διαφορετικο τύπο δεδομένων αρα κάθε φορά πρέπει να χειριζίμαστε αλλιώς τπ free για να μην έχουμε leaks)
		list->destroyValue(node->value);
		free(node);
		node = next;
	}
	//free τον χώρο που έχει δεσμευτεί για το struct
	free(list);
	
}


//αν εχει ππολλαπλες τιμες με την ιδια τιμη!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ListNode listFind(List list, Pointer value) {
	// διάσχιση όλης της λίστας, καλούμε την compare μέχρι να επιστρέψει 0
	for (ListNode node = list->head; node != NULL; node = node->next)
		//συγκρινουμε Graphnodes με βάση το id τους
		if (list->compare(value, listNodeValue(node)) == 0)
		//γυρναμε ListNode
			return node;
	return NULL;	// δεν υπάρχει
}


//για να μην υπάρχει πρόσβαση εκτος του αρχείου σε στοιχεία του struct list δημιουργούμε getters που θα χρειαστούν
ListNode listGetFirst(List list){
	return list->head;
}

ListNode listGetLast(List list){
	return list->last;
}

ListNode listGetNext(ListNode node){
	return node->next;
}

Pointer listNodeValue(ListNode node){
	return node->value;	
}