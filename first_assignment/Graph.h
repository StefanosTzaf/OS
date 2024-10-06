#pragma once
#include <string.h>
#include <stdlib.h>
#include "List.h"
#include <string.h>
#include "hash.h"

typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct vertex* Vertex;
//συναρτηση compare για την συγκριση των id των κομβων στην λιστα από κόμβους
int compareId(Pointer a, Pointer b);


//συναρτηση destroy για την λίστα από κόμβους.Καταστρεφει τον κόμβο με id value
void destroyGraphNode(Pointer value);

void destroyVertex(Pointer value);


Graph graphCreate();
void graphAddNode(Graph graph, char* id, Map map);
void addVertex(Graph graph, char* dateOfTransmission, int amount, char* id1, char* id2, Map map);

void displayGraph(Graph graph, Map map);