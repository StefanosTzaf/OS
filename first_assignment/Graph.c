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
    char* dateOfTransaction;
    int amount;
    GraphNode nodeDestination;
};
//------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------Συναρτήσεις που θα δώσπουμε στις λίστες---------------------------------------------------
//τυπου compare function για την σύγριση των id των κόμβων
int compareId(Pointer a, Pointer b){
    return strcmp((char*)a, (char*)b);
}

//Συνάρτηση που μας λέει στην λίστα απο Graphnodes πως να καταστρέψει κάθε κομβο της
void destroyGraphListNode(Pointer value){
    GraphNode node = value;
    free(node->id);
    //Καταστρέφουμε και την λείστα γειτνίασης του κάθε κόμβου
    listDestroy(node->adjencyLists);
    free(node);
}


//Συνάρτηση που μας λέει στην λίστα απο vertex πως να καταστρέψει κάθε κομβο της
void destroyAdjencyListNode(Pointer value){
    Vertex vertex = value;
    free(vertex->dateOfTransaction);
    vertex->nodeDestination = NULL;
    free(vertex);
}

//------------------------------------------------------------------------------------------------------------------------------

Graph graphCreate(){
    Graph graph = malloc(sizeof(*graph));
    //δημιουργούμαι μία λίστα από κόμβους
    graph->nodes = listCreate(destroyGraphListNode);
    return graph;
}



void graphAddNode(Graph graph, char* id, Map map){
    //Δημιουργούμe ένας κόμβος
    GraphNode node = malloc(sizeof(*node));
    node->id = malloc(strlen(id) + 1);
    strcpy(node->id, id);

    //Δημιουργούμe μία λίστα από κόμβους
    node->adjencyLists = listCreate(destroyAdjencyListNode);
    //Προσθέτω τον κόμβο στην λίστα από κόμβους
    listInsert(graph->nodes, node);
    //ενημέρωση και του hash table
    mapInsert(map, id, node);//Στο map αποθηκευουμε GraphNode
    graph->size++;

}

void destroyGraphNode(GraphNode node, Map map){
    //δίνουμε εντολή στο hash να μην δείχνει πιά σε αυτον τον κόμβο γιατι δεν υπάρχει πιά
    mapRemove(map, (node)->id);
    //διαγράφουμε τον κόμβο και από τον γράφο
    free(node->id);
    listDestroy(node->adjencyLists);
    free(node);
}

void destroyGraph(Graph graph, Map map){
    //όλη η δουλειά γίνεται στην destroyGraphListNode που περνάμε στην λίστα
    listDestroy(graph->nodes);
    map_destroy(map);
    free(graph);
}

void addVertex(Graph graph, char* dateOfTransaction, int amount, char* id1, char* id2, Map map){
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
    //Δημιουργώ έναν κόμβο με την ημερομηνία dateOfTransaction
    Vertex vertex = malloc(sizeof(*vertex));
    vertex->dateOfTransaction = malloc(strlen(dateOfTransaction) + 1);
    strcpy(vertex->dateOfTransaction, dateOfTransaction);
    vertex->amount = amount;
    vertex->nodeDestination = node2;
    //Προσθέτω τον κόμβο στην λίστα από κόμβους του κόμβου node1
    listInsert(node1->adjencyLists, vertex);
    
}

void displayGraph(Graph graph, Map map){
    printf("\nGraph with %d nodes\n\n", graph->size);
    ListNode nodes = listGetFirst(graph->nodes);

    while(nodes != NULL){

        GraphNode node = listNodeValue(nodes);
        printf("   Node with id: %s\n", node->id);
        ListNode adjencyLists = listGetFirst(node->adjencyLists);
        while(adjencyLists != NULL){
            Vertex vertex = listNodeValue(adjencyLists);
            printf("     Date of transmission: %s - Ammount of Transmission : %d$\n", vertex->dateOfTransaction, vertex->amount);
            printf("     Destination node: %s\n", ((vertex->nodeDestination)) -> id);
            adjencyLists = listGetNext(adjencyLists);
        }
    nodes = listGetNext(nodes);
    }

}