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
int compareGraphNodes(Pointer a, Pointer b){
    char* id1 = ((GraphNode)a)->id;
    char* id2 = ((GraphNode)b)->id;
    return strcmp(id1, id2);
}


//Μία συναλλαγή μπορεί να έχει ίδιο προορισμό και αφετηρία και ίδια ημερομηνία και ίδιο ποσό σναλλαγής.Άρα δεν υπάρχει κάποια ειδοποιός διαφορά μεταξύ των vertex
//επομένως σγκρίνουμε τους ίδιους τους Pointers!!
int compareVertices(Pointer a, Pointer b){
    return a == b;
}

//Συνάρτηση που μας λέει στην λίστα απο Graphnodes πως να καταστρέψει κάθε κομβο της
void destroyGraphListNode(Pointer nodeToDelete){
    GraphNode node = nodeToDelete;
    free(node->id);
    //Καταστρέφουμε και την λείστα γειτνίασης του κάθε κόμβου
    listDestroy(node->adjencyLists);
    free(node);
}


//Συνάρτηση που μας λέει στην λίστα απο vertex πως να καταστρέψει κάθε κομβο της
void destroyAdjencyListNode(Pointer vertexToDelete){
    Vertex vertex = vertexToDelete;
    free(vertex->dateOfTransaction);
    vertex->nodeDestination = NULL;
    free(vertex);
}

//------------------------------------------------------------------------------------------------------------------------------



Graph graphCreate(){
    Graph graph = malloc(sizeof(*graph));
    //δημιουργούμαι μία λίστα από κόμβους
    graph->nodes = listCreate(destroyGraphListNode, compareGraphNodes);
    return graph;
}



void graphAddNode(Graph graph, char* id, Map map){
    //Δημιουργούμe ένας κόμβος
    GraphNode node = malloc(sizeof(*node));
    node->id = malloc(strlen(id) + 1);
    strcpy(node->id, id);

    //Δημιουργούμe μία λίστα από κόμβους
    node->adjencyLists = listCreate(destroyAdjencyListNode, compareVertices);
    //Προσθέτω τον κόμβο στην λίστα από κόμβους
    listInsert(graph->nodes, node);
    //ενημέρωση και του hash table
    mapInsert(map, node->id, node);//Στο map αποθηκευουμε GraphNode
    graph->size++;

}


void removeGraphNode(char* id, Map map, Graph graph){
    //δίνουμε εντολή στο hash να μην δείχνει πιά σε αυτον τον κόμβο γιατι δεν υπάρχει πιά
    GraphNode node = mapFind(map, id);
    if(node == NULL){
        printf("Node with id %s does not exist\n", id);
        return;
    }
    listDeleteNode(graph->nodes, node);
    graph->size--;
}



void addVertex(Graph graph, char* dateOfTransaction, int amount, char* id1, char* id2, Map map){
    //Ψάχνω τον κόμβο με id1, μου επιστρέφει Graphnode*
    GraphNode node1 = mapFind(map, id1);
    GraphNode node2 = mapFind(map, id2);

    //Αν κάποιος κόμβος δεν υπάρχει τότε τον δημιουργώ
    if (node1 == NULL) {
        graphAddNode(graph, id1, map);
        node1 = mapFind(map, id1);

    }
    if (node2 == NULL) {
        graphAddNode(graph, id2, map);
        node2 = mapFind(map, id2);
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
            printf("     Date of transaction: %s - Ammount of Transaction : %d$\n", vertex->dateOfTransaction, vertex->amount);
            printf("     Destination node: %s\n", ((vertex->nodeDestination)) -> id);
            adjencyLists = listGetNext(adjencyLists);
        }
    nodes = listGetNext(nodes);
    }

}

void destroyGraph(Graph graph, Map map){
    //όλη η δουλειά γίνεται στην destroyGraphListNode που περνάμε στην λίστα
    listDestroy(graph->nodes);
    map_destroy(map);
    free(graph);
}