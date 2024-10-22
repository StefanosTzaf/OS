#include "Graph.h"

//------------------------------------------------------Ορισμός των δομών-------------------------------------------------------
//Ο γράφος αποτελείται από μία λίστα από GraphNodes
struct graph{
    List nodes;
    int size;
};

struct graph_node{
    char* id;
    //Λιστα απο edge η οποία έχει ακμές που ξεκιναν απο αυτόν τον κόμβο και καταλήγουν σε αλλον κόμβο
    List outgoingVertices;
    //Λίστα από edge που έχουν ακμές που καταλήγουν σε αυτόν τον κόμβο
    List incomingVertices;
    bool visited;
};

//μία ακμή αποτελείται από την ημερομηνία της συναλλαγής, το ποσό της συναλλαγής, δείκτες στον κόμβο προορισμού και αφετηρίας
struct edge{
    char* dateOfTransaction;
    int amount;
    GraphNode nodeDestination;
    GraphNode nodeOrigin;
};
//------------------------------------------------------------------------------------------------------------------------------






//------------------------------------------------------Συναρτήσεις που θα δώσουμε στις λίστες---------------------------------------------------
//τυπου compare function για την σύγριση των id των κόμβων
int compareGraphNodes(Pointer a, Pointer b){
    char* id1 = ((GraphNode)a)->id;
    char* id2 = ((GraphNode)b)->id;
    return strcmp(id1, id2);
}


int compareEdges(Pointer a, Pointer b){
    Edge edge1 = a;
    Edge edge2 = b;
    if(edge1->nodeDestination->id == edge2->nodeDestination->id && edge1->nodeOrigin->id == edge2->nodeOrigin->id){
        return 0;
    }
    return 1;
}

//list map node exei mesa graaph
int compareMapNodes(Pointer a, Pointer b){
    GraphNode node =  mapNodeValue((MapNode)b);
    return strcmp((char*)a, node->id);
}

//απλά free τον Pointer ,το εσωτερικό του Graphnode το διαγράφει η destroyGraphListNode
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
    ///άρα κάθε edge θα καλέσει την destroyIncomingEdge που κάνει ότι ακριβώς περιγράφεται παραπάνω!!

    listDestroy(node->incomingVertices);
    listDestroy(node->outgoingVertices);
    
    
    free(node->id);
    free(node);
}


//όταν θ΄έλουμε να αφαιρέσουμε μία ακμή :
void destroyEdge(Pointer edgeToDelete){
    Edge edge = edgeToDelete;
    GraphNode nodeDestination = edge->nodeDestination;
    GraphNode nodeOrigin = edge->nodeOrigin;

    if(nodeDestination->incomingVertices != NULL){
        listSetDestroyValue(nodeDestination->incomingVertices, NULL);
        listDeleteNode(nodeDestination->incomingVertices, edgeToDelete);
        listSetDestroyValue(nodeDestination->incomingVertices, destroyEdge);
    }

    if(nodeOrigin->outgoingVertices != NULL){
        listSetDestroyValue(nodeOrigin->outgoingVertices, NULL);
        listDeleteNode(nodeOrigin->outgoingVertices, edgeToDelete);
        listSetDestroyValue(nodeOrigin->outgoingVertices, destroyEdge);
    }
    free(edge->dateOfTransaction);
    free(edge);

}


//------------------------------------------------------------------------------------------------------------------------------



//------------------------------------------------------Συναρτήσεις για τον γράφο---------------------------------------------------

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
    node->visited = false;
    //Δημιουργούμe tις λίστες από ακμές στις outgoing περνάμε null γιατι όταν διαγράφουμε μία ακμη
    //η ακμή χρείάζεται μόνο μία φορά την συνάρτηση διαγραφής για να μην γίνει double free.Οπότε κάθε φορά που διαγράφουμε μία ακμή
    //μέσω του Origin θα βρίσκουμε την List απο outgoing vetrtex του και θα την καλούμε να διαγράψει την ακμή
    node->incomingVertices = listCreate(destroyEdge, compareEdges);
    node->outgoingVertices = listCreate(destroyEdge, compareEdges);

    //Προσθέτω τον κόμβο στην λίστα από κόμβους
    listInsert(graph->nodes, node);
    //ενημέρωση και του hash table
    mapInsert(map, node->id, node);//Στο map αποθηκευουμε GraphNode
    graph->size++;

}

void addEdge(Graph graph, char* dateOfTransaction, int amount, char* id1, char* id2, Map map){
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
    Edge edge = malloc(sizeof(*edge));
    edge->dateOfTransaction = malloc(strlen(dateOfTransaction) + 1);
    strcpy(edge->dateOfTransaction, dateOfTransaction);
    edge->amount = amount;
    edge->nodeDestination = node2;
    edge->nodeOrigin = node1;
    
    
    //Προσθέτω τον κόμβο στην λίστα από outgoing Vertices του κόμβου node1
    listInsert(node1->outgoingVertices, edge);
    //Προσθέτω τον κόμβο στην λίστα από incoming Vertices του κόμβου node2
    listInsert(node2->incomingVertices, edge);
}


void removeGraphNode(char* id, Map map, Graph graph){
    //δίνουμε εντολή στο hash να μην δείχνει πιά σε αυτον τον κόμβο γιατι δεν υπάρχει πιά
    GraphNode node = mapFind(map, id);
    if(node == NULL){
        return;
    }
    mapRemove(map, node->id);

    listDeleteNode(graph->nodes, node);
    
    graph->size--;
}


void removeEdge(char* id1, char* id2, Map map){

    Edge edgeToRemove = findEdge(id1, id2, map);

    if(edgeToRemove != NULL){

        destroyEdge(edgeToRemove);
    }

}


bool modifyEdge(char* id1, char* id2, char* date, int amount,char* date2, int amount2, Map map){
    //η find edge δεν μπορεί να μας βοηθήσει αυτή την στιγμή γιατί συγκρίνει βάση μόνο των Ids
    GraphNode origin = mapFind(map, id1);
    GraphNode destination = mapFind(map, id2);
    if(origin == NULL || destination == NULL){
        return 1;
    }
    //προσπέλαση μόνο του outgoing list του origin(ψάχνουμε μόνο ακμή από το id1->id2)
    for(ListNode node = listGetFirst(origin->outgoingVertices); node != NULL; node = listGetNext(node)){
        
        Edge edge = listNodeValue(node);
        if(strcmp(edge->nodeDestination->id, destination->id) == 0
            && strcmp(edge->dateOfTransaction, date) == 0 
            && edge->amount == amount){
            
            free(edge->dateOfTransaction);
            edge->dateOfTransaction = malloc(strlen(date2) + 1);
            strcpy(edge->dateOfTransaction, date2);
            edge->amount = amount2;
            return 0;
        }
    }
    return 1;
}



Edge findEdge(char* id1,char* id2, Map map){
    GraphNode node1 = mapFind(map, id1);
    GraphNode node2 = mapFind(map, id2);
    if(node1 == NULL || node2 == NULL){
        return NULL;
    }
    Edge temp = malloc(sizeof(*temp));
    temp->nodeDestination = node2;
    temp->nodeOrigin = node1;

    if(listFind(node1->outgoingVertices, temp) == NULL){
        free (temp);
        printf("------------");
        return NULL;
    }
    else{
        free (temp);
        return listNodeValue(listFind(node1->outgoingVertices, temp));
    }
}


void displayGraph(Graph graph, Map map){
    printf("\nGraph with %d nodes\n\n", graph->size);
    ListNode nodes = listGetFirst(graph->nodes);

    while(nodes != NULL){

        GraphNode node = listNodeValue(nodes);
        printf("   Node with id: %s\n\n", node->id);
        ListNode outgoingVertices = listGetFirst(node->outgoingVertices);
        printf("                         Outgoing Vertices:\n");
        while(outgoingVertices != NULL){
            Edge edge = listNodeValue(outgoingVertices);
            printf("     Date of transaction: %s - Ammount of Transaction : %d$\n", edge->dateOfTransaction, edge->amount);
            printf("     Destination node: %s\n", ((edge->nodeDestination)) -> id);
            outgoingVertices = listGetNext(outgoingVertices);
        }


        ListNode incomingVertices = listGetFirst(node->incomingVertices);
        printf("\n                         Incoming Vertices:\n");
        while(incomingVertices != NULL){
            Edge edge = listNodeValue(incomingVertices);
            printf("     Date of transaction: %s - Amount of Transaction : %d$\n", edge->dateOfTransaction, edge->amount);
            printf("     Origin node: %s\n", ((edge->nodeOrigin)) -> id);
            incomingVertices = listGetNext(incomingVertices);
        }
    printf("\n\n");
    nodes = listGetNext(nodes);

    }

}

void printToFile(Graph graph, FILE* file){
    ListNode listNode = listGetFirst(graph->nodes);
    //Κάθε κόμβο του γράφου
    while(listNode != NULL){

        GraphNode node = listNodeValue(listNode);
        //Εκτυπώνουμε μόνο τις Outgoing ακμλες για να μην εκτυπ΄σουμε κάθε ακμή 2 φορές
        if(listSize( node->outgoingVertices) == 0 && listSize(node->incomingVertices) == 0){
            fprintf(file, "%s (No transactions)\n", node->id);
        
        }
        ListNode outgoingVertices = listGetFirst(node->outgoingVertices);

        while(outgoingVertices != NULL){
            Edge edge = listNodeValue(outgoingVertices);
            fprintf(file,"%s ", edge->nodeOrigin->id);
            fprintf(file,"%s ", edge->nodeDestination->id);
            fprintf(file, "%d ", edge->amount);
            fprintf(file, "%s\n", edge->dateOfTransaction);

            outgoingVertices = listGetNext(outgoingVertices);
        }
        listNode = listGetNext(listNode);
    }
}

void destroyGraph(Graph graph){
    //όλη η δουλειά γίνεται στην destroyGraphListNode που περνάμε στην λίστα
    listDestroy(graph->nodes);
    free(graph);
}


void displayOutgoingEdges(char* id, Map map){
    GraphNode node = mapFind(map, id);
    if(node == NULL){
        return;
    }
    ListNode outgoingVertices = listGetFirst(node->outgoingVertices);
    while(outgoingVertices != NULL){
        Edge edge = listNodeValue(outgoingVertices);
        printf("   %s %s %d %s\n",edge->nodeOrigin->id, edge->nodeDestination->id, edge->amount, edge->dateOfTransaction);
        outgoingVertices = listGetNext(outgoingVertices);
    }
}


void displayIncomingEdges(char* id, Map map){
    GraphNode node = mapFind(map, id);
    if(node == NULL){
        return;
    }
    ListNode incomingVertices = listGetFirst(node->incomingVertices);
    while(incomingVertices != NULL){
        Edge edge = listNodeValue(incomingVertices);
        printf("   %s %s %d %s\n",edge->nodeOrigin->id, edge->nodeDestination->id, edge->amount, edge->dateOfTransaction);
        incomingVertices = listGetNext(incomingVertices);
    }
}

//αν το flag είναι 0 θέλουμε να βρούμε απλούς κύκλους αλλιώς σημαίνει οτι κλήθηκε για κύκλους με συνολικό ποσό
void findCircles(char* id, Graph graph, Map map, int minSum, bool flag) {
    GraphNode startNode = mapFind(map, id);
    if (startNode == NULL) {
        printf("   Non-existing node: %s\n\n", id);
        return;
    }

    List list = listCreate(NULL, compareGraphNodes);
    //8
    if(!flag){
        dfsPrintingCircles(startNode, startNode, list, minSum);
        printf("\n");
    }
    //9
    else{
        dfsPrintingCircles(startNode, startNode, list, minSum);
        printf("\n");
    }
    listDestroy(list);
}


void dfsPrintingCircles(GraphNode node, GraphNode startNode, List list, int minSum) {

    node->visited = true;
    //Αν δεν έχει καμία ακμή τότε δεν υπάρχει κύκλος
    if(node->outgoingVertices == NULL){
        return;
    }
    listInsert(list, node);
    bool foundCircle = false;   
    //για κάθε εξερχόμενη ακμή
    for (ListNode outgoingVertices = listGetFirst(node->outgoingVertices);
        outgoingVertices != NULL; outgoingVertices = listGetNext(outgoingVertices)){

        Edge edge = listNodeValue(outgoingVertices);
        //για τους κύκλους μεγαλύτερους από ένα συγκεκριμένο ποσό, αν η διαδρομή που ψάχνουμε κ
        //έχει ακμή μικρότερη από αυτόν τον αριθμό , συνεχίζουμε με την επόμενη
        if(edge->amount < minSum){
            continue;
        }
        GraphNode child = edge->nodeDestination;

        //Αν ο κόμβος είναι που ξεκινάμε είναι ίδιος με τον κόμβο που έχουμε φτάσει τότε έχει βρεθεί κύκλος
        if (strcmp(child->id, startNode->id) == 0) {
            printf("   ");
            foundCircle = true;
            listInsert(list, child);
            ListNode circleNode = listGetFirst(list);
    	    //Εκτύπωση του κύκλου
            while (circleNode != NULL) {
                GraphNode node1 = listNodeValue(circleNode);
                printf("%s ", node1->id);
                circleNode = listGetNext(circleNode);
                if(circleNode != NULL){
                    printf("-> ");
                }
            }

            printf("\n");
            //αφαιρούμε τον κόμβο θέλουμε μόνο απλους κυκλους , ανσυνέχιζε θα εβρισκε και κύκλους της μορφής
            //2->3->4->2->6->2
            listRemoveLast(list);
        } 
        
        else if (!child->visited) {
            //Αν δεν έχει επισκεφτεί τον κόμβο τότε εμβαθύνουμε στο παιδί
            dfsPrintingCircles(child, startNode, list, minSum);
        }

        //Αν ο κόμβος είναι ο ίδιος με τον αρχικό KAI έχει βρεθεί κύκλος ,δηλαδή δεν είμαστε στον αρχικό κόμβο
        // γιατι από εκεί ξεκινάμε αλλα γιατί έχουμε ξαναφτάσει, τότε σταματάμε(θέλουμε απλούς κυκλους)
        if((strcmp(node->id, startNode->id) == 0 )&& foundCircle){
            break;
        }
    }

    node->visited = false;
    listDeleteNode(list, node);
}

void findPath(Graph graph, char* id1, char* id2, Map map){
    GraphNode node1 = mapFind(map, id1);

    List list = listCreate(NULL, compareGraphNodes);
    bool* found;
    found = malloc(sizeof(bool));
    *found = false;
    dfsPath(node1, id2, list, map, found);
    if(!(*found)){
        printf("   No path found\n");
    }
    free(found);
    listDestroy(list);
    
}

void dfsPath(GraphNode node, char* destination, List list, Map map, bool* found){
    node->visited = true;
    //Αν δεν έχει καμία ακμή τότε δεν υπάρχει μονοπάτι
    if(node->outgoingVertices == NULL){
        return;
    }
    listInsert(list, node);  
    //για κάθε εξερχόμενη ακμή
    for (ListNode outgoingVertices = listGetFirst(node->outgoingVertices);
        outgoingVertices != NULL; outgoingVertices = listGetNext(outgoingVertices)){

        Edge edge = listNodeValue(outgoingVertices);
        GraphNode child = edge->nodeDestination;

        //Αν ο κόμβος είναι που ξεκινάμε είναι ίδιος με τον κόμβο destination τοτε βρήκαμε μονοπάτι
        if (strcmp(child->id, destination) == 0) {
            printf("   ");
            (*found) = true;
            listInsert(list, child);
            ListNode pathNode = listGetFirst(list);
    	    //Εκτύπωση του μονοπατιού
            while (pathNode != NULL) {
                GraphNode node1 = listNodeValue(pathNode);
                printf("%s ", node1->id);
                pathNode = listGetNext(pathNode);
                if(pathNode != NULL){
                    printf("-> ");
                }
            }

            printf("\n");
            listRemoveLast(list);
        } 
        
        else if (!child->visited) {
            //Αν δεν έχει επισκεφτεί τον κόμβο τότε εμβαθύνουμε στο παιδί
            dfsPath(child, destination, list, map, found);
        }

        if(*found){
            break;
        }
    }
    node->visited = false;
    listDeleteNode(list, node);
}