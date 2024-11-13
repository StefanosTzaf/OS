//--------------------------------------Implementation of a doubly linked list-----------------------------------------
#include <stdlib.h>
#include <stdio.h>

typedef void* Pointer;
// We want our list to be generic, so it will contain pointers to elements of type void
typedef struct list* List;
typedef struct list_node* ListNode;

// We use a function pointer for comparing 2 elements, it returns:
// 0 if they are equal based on the function (not necessarily *a == *b, we might be dealing with structs so the compare function will be different and this is its value)
// It is used in listFind
typedef int (*CompareFunc)(Pointer a, Pointer b);

typedef void (*DestroyFunc)(Pointer value);

// (O(1))
List listCreate(DestroyFunc destroyValue, CompareFunc compare);

// (O(1))
void listInsert(List list, Pointer value);

// (O(n)) due to ListFind
void listDeleteNode(List list, Pointer value);
// O(1)
void listRemoveLast(List list);

// (O(n))
void listDestroy(List list);

// (O(n))
ListNode listFind(List list, Pointer value);

// (O(1))
ListNode listGetLast(List list);
ListNode listGetFirst(List list);
ListNode listGetNext(ListNode node);
Pointer listNodeValue(ListNode node);
int listSize(List list);
void listSetDestroyValue(List list, DestroyFunc destroyValue);

