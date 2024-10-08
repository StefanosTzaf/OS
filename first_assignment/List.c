#include "List.h"

//Η λιστα θα είναι ένας pointer σε αυτό το struct
struct list {
	ListNode head;
	ListNode last;
	int size;
	DestroyFunc destroyValue;
};

struct list_node {
	ListNode next;// Δείκτης στον επόμενο κόμβο
	Pointer value;
};


List listCreate(DestroyFunc destroyValue) {
	// Πρώτα δημιουργούμε το stuct
	List list = malloc(sizeof(*list));
	list->size = 0;
	list->last = NULL;
	list->head = NULL;
	list->destroyValue = destroyValue;
	return list;
}

void listInsert(List list, Pointer value) {
	ListNode new = malloc(sizeof(*new));
	new->value = value;
	new->next = NULL;
	//Εαν υπάρχει κόμβος μέσα στην λίστα
	if (list->last != NULL){
		list->last->next = new;
	}
	else{
		list->head = new;
	}
	list->last = new;
	list->size++;
}


void deleteNode(List list, Pointer value, CompareFunc compare) {
    ListNode current = list->head;
    ListNode prev = NULL;

    // Έλεγχος αν η λίστα είναι άδεια
    if (current == NULL) {
        return;
    }

    // Έλεγχος αν η κεφαλή περιέχει την τιμή που θέλουμε να διαγράψουμε
    if (current != NULL && compare(current->value, value) == 0) {
        list->head = current->next;  // Η νέα κεφαλή γίνεται ο επόμενος κόμβος
        free(current);              // Απελευθέρωση μνήμης της παλιάς κεφαλής
        return;
    }

    // Αναζήτηση του κόμβου με την τιμή που πρέπει να διαγραφεί
    while (current != NULL && compare(current->value, value)) {
        prev = current;
        current = current->next;
    }

    // Αν ο κόμβος δεν βρέθηκε
    if (current == NULL) {
        return;
    }

    // Παρακάμψτε τον κόμβο
    prev->next = current->next;

    // Απελευθέρωση μνήμης του κόμβου που διαγράφηκε
    list->destroyValue(current->value);
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


Pointer listFind(List list, Pointer value, CompareFunc compare) {
	ListNode node = listFindNode(list, value, compare);
	return node == NULL ? NULL : node->value;
}

//αν εχει ππολλαπλες τιμες με την ιδια τιμη!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
ListNode listFindNode(List list, Pointer value, CompareFunc compare) {
	// διάσχιση όλης της λίστας, καλούμε την compare μέχρι να επιστρέψει 0
	for (ListNode node = list->head; node != NULL; node = node->next)
		if (compare(value, node->value) == 0)
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