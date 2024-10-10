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
    List outgoingVertices;
    //Λίστα από vertex που έχουν ακμές που καταλήγουν σε αυτόν τον κόμβο
    List incomingVertices;

};

struct vertex{
    char* dateOfTransaction;
    int amount;
    GraphNode nodeDestination;
    GraphNode nodeOrigin;
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
    Vertex vertex1 = a;
    Vertex vertex2 = b;
    if(vertex1->nodeDestination->id == vertex2->nodeDestination->id && vertex1->nodeOrigin->id == vertex2->nodeOrigin->id){
        return 0;
    }
    return 1;
}

//list map node exei mesa graaph
int compareMapNodes(Pointer a, Pointer b){
    GraphNode node =  mapNodeValue((MapNode)b);
    return strcmp((char*)a,node->id);
}

void destroyMapNodes(Pointer value){
    free(value);
}




//Συνάρτηση που μας λέει στην λίστα απο Graphnodes πως να καταστρέψει κάθε κομβο της
void destroyGraphListNode(Pointer nodeToDelete){
    GraphNode node = nodeToDelete;
    //Πρέπει να διαγράψουμε όλες τις incoming vertices αλλα και απο τις λιστες outgoing vertices της προελευσης
    //πχ αν διαγράφουμε τον κομβο 3 και έχει μια ακμη 5->3 τότε πρέπει να διαγράψουμε την ακμή απο την incoming list του 5 και την outgoing list του 3
    //αντίστοιχα ολες τις ouygoing vertices αλλα λαι απο τις incoming vertices του προορισμού

    //!!Εδώ φαίνεται η αξία της generic υλοποίησης εμείς απλά καλούμε την list destroy καια αυτη με τη σειρά της όταν πάει να διαγράψει κάθε κόμβο
    ///άρα κάθε vertex θα καλέσει την destroyIncomingVertex που κάνει ότι ακριβώς περιγράφεται παραπάνω!!

    listDestroy(node->outgoingVertices);

    listDestroy(node->incomingVertices);

    free(node->id);
    free(node);
}


//όταν θ΄έλουμε να αφαιρέσουμε μία ακμή :
void destroyVertex(Pointer vertexToDelete){
    Vertex vertex = vertexToDelete;
    GraphNode nodeOrigin = vertex->nodeOrigin;
    GraphNode nodeDestination = vertex->nodeDestination;

    //θέτουμε προσωρινα τις τιμες ως null έτσι ώστε η listdeletenode να αφαιρέσει απλα τον Pointer απο τις δύο λ΄ίστες γιατι αλλιως θα ξανακαλουσε την destroy value η οποία είναι αυτη η συνάρτηση
    //και θα είχαμε προλήματα Double free και κυκλου
    listSetDestroyValue(nodeOrigin->outgoingVertices, NULL);
    listSetDestroyValue(nodeDestination->incomingVertices, NULL);
   listDeleteNode(nodeOrigin->outgoingVertices, vertexToDelete);

    //1.Πρέπει να την αφαιρέσουμε από την λίστα των εξερχόμενων ακμών του κόμβου που ξεκινάει
    listDeleteNode(nodeDestination->incomingVertices, vertexToDelete);

    listSetDestroyValue(nodeOrigin->outgoingVertices, destroyVertex);
    listSetDestroyValue(nodeDestination->incomingVertices, destroyVertex);
    //3.Να απελευθερώσουμε την μνήμη(οι λίστες απλά περιέχουν pointers)
    
    free(vertex->dateOfTransaction);
    free(vertex);

}

//------------------------------------------------------------------------------------------------------------------------------



Graph graphCreate(){
    Graph graph = malloc(sizeof(*graph));
    //δημιουργούμαι μία λίστα από κόμβους
    graph->nodes = listCreate(destroyGraphListNode, compareGraphNodes);
    graph->size = 0;
    return graph;
}



void graphAddNode(Graph graph, char* id, Map map){
    //Δημιουργούμe ένας κόμβος
    GraphNode node = malloc(sizeof(*node));
    node->id = malloc(strlen(id) + 1);
    strcpy(node->id, id);

    //Δημιουργούμe tις λίστες από ακμές στις outgoing περνάμε null γιατι όταν διαγράφουμε μία ακμη
    //η ακμή χρείάζεται μόνο μία φορά την συνάρτηση διαγραφής για να μην γίνει double free.Οπότε κάθε φορά που διαγράφουμε μία ακμή
    //μέσω του Origin θα βρίσκουμε την List απο outgoing vetrtex του και θα την καλούμε να διαγράψει την ακμή
    node->outgoingVertices = listCreate(destroyVertex, compareVertices);
    node->incomingVertices = listCreate(destroyVertex, compareVertices);

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
    mapRemove(map, node->id);

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
    vertex->nodeOrigin = node1;
    
    //Προσθέτω τον κόμβο στην λίστα από outgoing Vertices του κόμβου node1
    listInsert(node1->outgoingVertices, vertex);
    //Προσθέτω τον κόμβο στην λίστα από incoming Vertices του κόμβου node2
    listInsert(node2->incomingVertices, vertex);
    
}

// void removeVertex(char* id1, char* id2, Map map){
//     GraphNode node1 = mapFind(map, id1);
//     GraphNode node2 = mapFind(map, id2);
//     if(node1 == NULL || node2 == NULL){
//         printf("One of the nodes does not exist\n");
//         return;
//     }
//     Vertex vertex = malloc(sizeof(*vertex));
//     vertex->dateOfTransaction = malloc(strlen(dateOfTransaction) + 1);
//     strcpy(vertex->dateOfTransaction, dateOfTransaction);
//     vertex->amount = amount;
//     vertex->nodeDestination = node2;
//     vertex->nodeOrigin = node1;

//     listDeleteNode(node1->outgoingVertices, vertex);
//     listDeleteNode(node2->incomingVertices, vertex);
//     free(vertex->dateOfTransaction);
//     free(vertex);
// }

// Vertex findVertex(char* id1,char* id2, Map map){
//     GraphNode node1 = mapFind(map, id1);
//     GraphNode node2 = mapFind(map, id2);
//     if(node1 == NULL || node2 == NULL){
//         printf("One of the nodes does not exist\n");
//         return NULL;
//     }

//     ListNode node = listFind(node1->outgoingVertices, vertex);
//     if(node == NULL){
//         printf("Vertex does not exist\n");
//         return NULL;
//     }
//     return listNodeValue(node);
// }


void displayGraph(Graph graph, Map map){
    printf("\nGraph with %d nodes\n\n", graph->size);
    ListNode nodes = listGetFirst(graph->nodes);

    while(nodes != NULL){

        GraphNode node = listNodeValue(nodes);
        printf("   Node with id: %s\n\n", node->id);
        ListNode outgoingVertices = listGetFirst(node->outgoingVertices);
        printf("                         Outgoing Vertices:\n");
        while(outgoingVertices != NULL){
            Vertex vertex = listNodeValue(outgoingVertices);
            printf("     Date of transaction: %s - Ammount of Transaction : %d$\n", vertex->dateOfTransaction, vertex->amount);
            printf("     Destination node: %s\n", ((vertex->nodeDestination)) -> id);
            outgoingVertices = listGetNext(outgoingVertices);
        }


        ListNode incomingVertices = listGetFirst(node->incomingVertices);
        printf("\n                         Incoming Vertices:\n");
        while(incomingVertices != NULL){
            Vertex vertex = listNodeValue(incomingVertices);
            printf("     Date of transaction: %s - Ammount of Transaction : %d$\n", vertex->dateOfTransaction, vertex->amount);
            printf("     Origin node: %s\n", ((vertex->nodeOrigin)) -> id);
            incomingVertices = listGetNext(incomingVertices);
        }
    printf("\n\n");
    nodes = listGetNext(nodes);

    }

}

void destroyGraph(Graph graph){
    //όλη η δουλειά γίνεται στην destroyGraphListNode που περνάμε στην λίστα
    listDestroy(graph->nodes);
    free(graph);
}