#include "hash.h"

typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct vertex* Vertex;

//συναρτηση compare για την συγκριση των id των κομβων στην λιστα από κόμβους
int compareGraphNodes(Pointer a, Pointer b);

//συνάρτηση compare για vertex
int compareVertices(Pointer a, Pointer b);

//συνάρτηση compare για την σύγκριση των id των κόμβων στο hash table
int compareMapNodes(Pointer a, Pointer b);

//καταστρέφει το value από κόμβους της λίστα του γράφου
void destroyGraphListNode(Pointer nodeToDelete);

//καταστρέφει το value απο τους κόμβους των λιστών από vertex
void destroyVertex(Pointer vertexToDelete);

//καταστρέφει το value από τους κόμβους των λιστών hash table
void destroyMapNodes(Pointer value);


Graph graphCreate();

void graphAddNode(Graph graph, char* id, Map map);
void addVertex(Graph graph, char* dateOfTransmission, int amount, char* id1, char* id2, Map map);

void removeGraphNode(char* id, Map map, Graph graph);
void removeVertex(char* id1, char* id2, Map map);

bool modifyVertex(char* id1, char* id2, char* date, int amount,char* date2, int amount2, Map map);


//βρίσκει ακμή βάση μόνο των id των κόμβων (χρειάζεται για την removeVertex)
Vertex findVertex(char* id1,char* id2, Map map);


void destroyGraph(Graph graph);

void displayGraph(Graph graph, Map map);

void printToFile(Graph graph, FILE* file);

void displayOutgoingEdges(char* id, Map map);
void displayIncomingEdges(char* id, Map map);

void findCircles(char* id,Graph graph, Map map);