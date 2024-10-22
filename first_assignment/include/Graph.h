#include "Map.h"
typedef struct graph* Graph;
typedef struct graph_node* GraphNode;
typedef struct edge* Edge;


//------------------------------------------------------- Συναρτήσεις για τις λίστες ------------------------------------------

//Ο(1)
//συναρτηση compare για την συγκριση των id των κομβων στην λιστα από κόμβους
int compareGraphNodes(Pointer a, Pointer b);
//Ο(1)
//συνάρτηση compare για edge
int compareEdges(Pointer a, Pointer b);
//Ο(1)
//συνάρτηση compare για την σύγκριση των id των κόμβων στο hash table
int compareMapNodes(Pointer a, Pointer b);

//O(n + m) κόμβοι της λίστας απο GraphNode και m οι ακμ΄ές 
//καταστρέφει το value από κόμβους της λίστας του γράφου δηλαδή Graphnode
void destroyGraphListNode(Pointer nodeToDelete);

//O(n) οπου n οι ακμές μια λιστας με edges
//καταστρέφει το value απο τους κόμβους των λιστών από edge
void destroyEdge(Pointer edgeToDelete);

//Ο(1)
//καταστρέφει το value από τους κόμβους των λιστών hash table
void destroyMapNodes(Pointer value);

//---------------------------------------------------------------------------------------------------------------------------------


//Ο(1)
Graph graphCreate();

//Ο(1)
void graphAddNode(Graph graph, char* id, Map map);
void addEdge(Graph graph, char* dateOfTransmission, int amount, char* id1, char* id2, Map map);

//Ο(n + m) λογω της destroyGraphListNode
void removeGraphNode(char* id, Map map, Graph graph);

//Ο(n) λόγω της findEdge
void removeEdge(char* id1, char* id2, Map map);

//βρίσκει ακμή βάση μόνο των id των κόμβων (χρειάζεται για την removeEdge)
//Ο(n) n οι ακμές του κόμβου(outgoing)
Edge findEdge(char* id1,char* id2, Map map);

//O(n)
bool modifyEdge(char* id1, char* id2, char* date, int amount,char* date2, int amount2, Map map);

//O(n) n οι ακμές
void displayOutgoingEdges(char* id, Map map);
void displayIncomingEdges(char* id, Map map);

//O(n*m)
void destroyGraph(Graph graph);
void printToFile(Graph graph, FILE* file);

//Ο(m + n) γνωστή πολυπλοκότητα dfs
void findCircles(char* id,Graph graph, Map map, int minSum, bool flag);
void dfsPrintingCircles(GraphNode node, GraphNode startNode, List list, int minSum);
void findPath(Graph graph, char* id1, char* id2, Map map);
void dfsPath(GraphNode node, char* destination, List list, Map map, bool* found);