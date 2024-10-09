//--------------------------------------Υλοποίση διπλά συνδεδεμένης λίστας-----------------------------------------
#include <stdlib.h>

//H λίστα μας θελουμε να ειναι generic γιαυτο θα περιέχει pointer σε στοιχεία τύπου void(είτε Graphnode είτε vertex)
//τα Struct ορίζονται incomplete η υλοποιίηση τους είναι στο List.c.Για να μην μπορούν οι χρήστες
// (στην προκειμένη περίπτωση ο Γράφος) να έχουν πρόσβαση στα πεδία τους και να μπορούν να τα μεταβάλουν απευθείας
typedef void* Pointer;
typedef struct list* List;
typedef struct list_node* ListNode;

//Χρησιμοποιύμε δείκτη σε συνάρτηση για συγκριση 2 στοιχείων, επιστρέφει:
// 0 αν είναι ίσα βάση της συνάρτησης (όχι αναγκαστικά *a == *b μπορει να έχουμε να κάνουμε με struct αρα η compare θα είναι διαφορετική και αυτή είναι η αξία της)
//Χρησιμοποιείται στην listFind
typedef int (*CompareFunc)(Pointer a, Pointer b);

//Πάρόμοια η Destroy διαφέρει ανάλογα που δείχνει ο δείκτης. Πχ στην εργασία χρησιμοποιούμε δυο είδη λίστας που δεσμεύουν διαφορετική μν΄ήμη.
//Η destroy είναι  στοθχείο του struct list
typedef void (*DestroyFunc)(Pointer value);

List listCreate(DestroyFunc destroyValue, CompareFunc compare);

void listInsert(List list, Pointer value);

void listDeleteNode(List list, Pointer value);

void listDestroy(List list);

//ψαχνει την τιμη value στην λιστα list με την βοηθεια της συναρτησης compare
ListNode listFind(List list, Pointer value);

ListNode listGetLast(List list);
ListNode listGetFirst(List list);
ListNode listGetNext(ListNode node);

int listSize(List list);
Pointer listNodeValue(ListNode node);