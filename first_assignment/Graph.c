#include "Graph.h"
#include <stdio.h>

//------------------------------------------------------Ορισμός των δομών-------------------------------------------------------
//Ο γράφος αποτελείται από μία λίστα από GraphNodes
struct graph{
    List nodes;
    int size;
};

struct graph_node{
    char* id;
    //Λιστα απο vertex η οποία έχει ακμές που ξεκιναν απο αυτόν τον κόμβο και καταλήγουν σε αλλον κόμβο
    List adjencyLists;
};

struct vertex{
    char* dateOfTransmission;
    int amount;
    GraphNode nodeDestination;
};
//------------------------------------------------------------------------------------------------------------------------------



Graph graphCreate(){
    Graph graph = malloc(sizeof(*graph));
    //δημιουργούμαι μία λίστα από κόμβους
    graph->nodes = listCreate(destroyGraphNode);
    return graph;
}

int compareId(Pointer a, Pointer b){
    return strcmp((char*)a, (char*)b);
}


void graphAddNode(Graph graph, char* id, Map map){
    //Δημιουργούμe ένας κόμβος
    GraphNode node = malloc(sizeof(*node));
    
    node->id = id;
    //Δημιουργούμe μία λίστα από κόμβους
    node->adjencyLists = listCreate(destroyVertex);
    //Προσθέτω τον κόμβο στην λίστα από κόμβους
    listInsert(graph->nodes, node);
    //ενημέρωση και του hash table
    mapInsert(map, id, node);//Στο map αποθηκευουμε GraphNode
    graph->size++;

}

void destroyGraphNode(Pointer value){
    GraphNode node = value;
    free(node->id);
    listDestroy(node->adjencyLists);
    free(node);
}



void addVertex(Graph graph, char* dateOfTransmission, int amount, char* id1, char* id2, Map map){
    //Ψάχνω τον κόμβο με id1, μου επιστρέφει Graphnode*
    GraphNode node1 = mapFind(map, id1);
 

    GraphNode node2 = mapFind(map, id2);
    if (node1 == NULL) {
        printf("Error: Node with id %s not found!\n", id1);
        return;
    }
    if (node2 == NULL) {
        printf("Error: Node with id %s not found!\n", id2);
        return;
    }
    //Δημιουργώ έναν κόμβο με την ημερομηνία dateOfTransmission
    Vertex vertex = malloc(sizeof(*vertex));
    vertex->dateOfTransmission = dateOfTransmission;
    vertex->amount = amount;
    vertex->nodeDestination = node2;
    //Προσθέτω τον κόμβο στην λίστα από κόμβους του κόμβου node1
    listInsert(node1->adjencyLists, vertex);
    
}

void destroyVertex(Pointer value){
    Vertex vertex = value;
    free(vertex);
}

void displayGraph(Graph graph, Map map){
    printf("\nGraph with %d nodes\n\n", graph->size);
    ListNode nodes = listGetFirst(graph->nodes);

    while(nodes != LIST_EOF){

        GraphNode node = listNodeValue(nodes);
        printf("   Node with id: %s\n", node->id);
        ListNode adjencyLists = listGetFirst(node->adjencyLists);
        while(adjencyLists != LIST_EOF){
            Vertex vertex = listNodeValue(adjencyLists);
            printf("     Date of transmission: %s - Ammount of Transmission : %d$\n", vertex->dateOfTransmission, vertex->amount);
            printf("     Destination node: %s\n", ((vertex->nodeDestination)) -> id);
            adjencyLists = listGetNext(adjencyLists);
        }
    nodes = listGetNext(nodes);
    }

}