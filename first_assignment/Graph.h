#pragma once
#include <string.h>
#include <stdlib.h>
#include "List.h"
#include "hash.h"

typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct vertex* Vertex;


//συναρτηση compare για την συγκριση των id των κομβων στην λιστα από κόμβους(διαφέρει απο την compareId του map εκείνη δέχεται τα Id)
int compareGraphNodes(Pointer a, Pointer b);
//συνάρτηση compare για vertex
int compareVertices(Pointer a, Pointer b);

//καταστρέφει κόμβους της λίστα του γράφου
void destroyGraphListNode(Pointer nodeToDelete);

//καταστρέφει τους κόμβους της λίστας από vertex
void destroyVertex(Pointer vertexToDelete);


int compareMapNodes(Pointer a, Pointer b);
void destroyMapNodes(Pointer value);

Graph graphCreate();

void graphAddNode(Graph graph, char* id, Map map);

//αφαιρεί έναν κόμβο βάση του Id του
void removeGraphNode(char* id, Map map, Graph graph);

void addVertex(Graph graph, char* dateOfTransmission, int amount, char* id1, char* id2, Map map);

void displayGraph(Graph graph, Map map);

void destroyGraph(Graph graph);

Vertex findVertex(char* id1,char* id2, Map map);
void removeVertex(char* id1, char* id2, Map map);