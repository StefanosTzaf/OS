#include <stdlib.h>

//H λίστα μας θελουμε να ειναι generic γιαυτο θα περιέχε pointer σε στοιχεία τύπου void(είτε Graphnode είτε vertex )
typedef void* Pointer;
typedef struct list* List;
typedef struct list_node* ListNode;

// Δείκτης σε συνάρτηση που συγκρίνει 2 στοιχεία a και b και επιστρέφει:
// 0  αν a και b είναι ίσα βάση της συνάρτησης, επομένως αν θέλουμε να συγκρινουμε id πελατών θα δώσουμε την συνάρτηση compareId
typedef int (*CompareFunc)(Pointer a, Pointer b);

// Δείκτης σε συνάρτηση που καταστρέφει ένα στοιχείο value
typedef void (*DestroyFunc)(Pointer value);

List listCreate(DestroyFunc destroyValue);

void listInsert(List list, Pointer value);

void listDeleteNode(List list, Pointer value ,CompareFunc compare);

void listDestroy(List list);

//ψαχνει την τιμη value στην λιστα list με την βοηθεια της συναρτησης compare
ListNode listFind(List list, Pointer value, CompareFunc compare);

ListNode listGetLast(List list);
ListNode listGetFirst(List list);
ListNode listGetNext(ListNode node);
Pointer listNodeValue(ListNode node);