//--------------------------------------Υλοποίηση διπλά συνδεδεμένης λίστας-----------------------------------------
#include <stdlib.h>
#include <stdio.h>

typedef void* Pointer;
//H λίστα μας θελουμε να ειναι generic γιαυτο θα περιέχει pointer σε στοιχεία τύπου void(είτε Graphnode είτε vertex)
typedef struct list* List;
typedef struct list_node* ListNode;

//Χρησιμοποιύμε δείκτη σε συνάρτηση για συγκριση 2 στοιχείων, επιστρέφει:
// 0 αν είναι ίσα βάση της συνάρτησης (όχι αναγκαστικά *a == *b μπορει να έχουμε να κάνουμε με struct αρα η compare θα είναι διαφορετική και αυτή είναι η αξία της)
//Χρησιμοποιείται στην listFind
typedef int (*CompareFunc)(Pointer a, Pointer b);

//Πάρόμοια η Destroy διαφέρει ανάλογα που δείχνει ο δείκτης. Πχ στην εργασία χρησιμοποιούμε τρία είδη λίστας που δεσμεύουν διαφορετική μν΄ήμη.
//Η destroy είναι  στοθχείο του struct list
typedef void (*DestroyFunc)(Pointer value);

//(Ο(1))
List listCreate(DestroyFunc destroyValue, CompareFunc compare);

//(Ο(1))
void listInsert(List list, Pointer value);

//(Ο(n)) λόγω της ListFind
void listDeleteNode(List list, Pointer value);

//(Ο(n))
void listDestroy(List list);

//(Ο(n))
ListNode listFind(List list, Pointer value);

//(Ο(1))
ListNode listGetLast(List list);
ListNode listGetFirst(List list);
ListNode listGetNext(ListNode node);
Pointer listNodeValue(ListNode node);
int listSize(List list);
void listSetDestroyValue(List list, DestroyFunc destroyValue);

void listRemoveLast(List list);