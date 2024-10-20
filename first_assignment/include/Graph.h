#include "hash.h"
typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct edge* Edge;

//συναρτηση compare για την συγκριση των id των κομβων στην λιστα από κόμβους
int compareGraphNodes(Pointer a, Pointer b);

//συνάρτηση compare για edge
int compareEdges(Pointer a, Pointer b);

//συνάρτηση compare για την σύγκριση των id των κόμβων στο hash table
int compareMapNodes(Pointer a, Pointer b);

//καταστρέφει το value από κόμβους της λίστα του γράφου
void destroyGraphListNode(Pointer nodeToDelete);

//καταστρέφει το value απο τους κόμβους των λιστών από edge
void destroyEdge(Pointer edgeToDelete);

//καταστρέφει το value από τους κόμβους των λιστών hash table
void destroyMapNodes(Pointer value);


Graph graphCreate();

void graphAddNode(Graph graph, char* id, Map map);
void addEdge(Graph graph, char* dateOfTransmission, int amount, char* id1, char* id2, Map map);

void removeGraphNode(char* id, Map map, Graph graph);
void removeEdge(char* id1, char* id2, Map map);

bool modifyEdge(char* id1, char* id2, char* date, int amount,char* date2, int amount2, Map map);


//βρίσκει ακμή βάση μόνο των id των κόμβων (χρειάζεται για την removeEdge)
Edge findEdge(char* id1,char* id2, Map map);


void destroyGraph(Graph graph);

void displayGraph(Graph graph, Map map);

void printToFile(Graph graph, FILE* file);

void displayOutgoingEdges(char* id, Map map);
void displayIncomingEdges(char* id, Map map);

void findCircles(char* id,Graph graph, Map map, int minSum, bool flag);
void dfsPrintingCircles(GraphNode node, GraphNode startNode, List list);



void dfsPrintingCircles2(GraphNode node, GraphNode startNode, List list, int minSum);