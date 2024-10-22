#include "Graph.h"
#include <getopt.h>
#include <stdbool.h>

size_t currentMemoryUsage = 0;

// // Wrapper συναρτήσεις για να μπορούμε να μετράμε σε έναν global counter την μνήμη που χρησιμοποιοώυμε
// void* myMalloc(size_t size) {
//     void* ptr = malloc(size);
//     if (ptr != NULL) {
//         currentMemoryUsage += size;
//     }
//     return ptr;
// }

// void myFree(void* ptr, size_t size) {
//     if (ptr != NULL) {
//         currentMemoryUsage -= size;
//         free(ptr);
//     }
// }

int main(int argc, char *argv[]){


  int option;

  //Για να αποθηκεύσουμε το όνομα του αρχείου εισόδου και εξόδου από την γραμμή εντολών
  char* inputFile = NULL;
  char* outputFile = NULL;

  if(argc != 5){
    fprintf(stderr, "Error : run with format -i inputfile -o outputfile\n");
    return 1;
  }

  while((option = getopt(argc, argv, "i:o:")) != -1){
    if(option == 'i'){
      inputFile = optarg;
    }
    else if(option == 'o'){
      outputFile = optarg;
    } 
    else{
      return 1;
    }
  }

  FILE* file;
  file = fopen(inputFile, "r");
  if(file == NULL){
    fprintf(stderr, "Error opening file\n");
    return 1;
  }

  char line[256];     //για το διαβασμα μίας γραμμής του αρχείου
  int counter = 0;    //Ποσες γραμμες έχει το αρχείο για το μέγεθος του Hash table
  while (fgets(line, sizeof(line), file) != NULL) {
      counter++;
  }

  Graph graph = graphCreate();
  Map map = mapCreate(compareMapNodes, destroyMapNodes, counter);

  rewind(file);   //από την αρχή του αρχείου
  //με την συνάρτηση strtok που ανααφέρθηκε στο φροντηστήριο διαβάζουμε τα δεδομένα από το αρχείο
  //που χωρίζονται με κενά. Στην αρχή περνάμε ως ορισμα την γραμμή που θέλουμε να διαβάσουμε και μετά
  //NULL (η συνάρτηση θα ασυνεχίσει στην γραμμή που την κάλεσε)
  while (fgets(line, sizeof(line), file) != NULL) {
    char* id1;
    char* id2;
    int amount;
    char* date;
    id1 = strtok(line, " ");
    id2 = strtok(NULL, " ");
    //μετατροπη σε integer
    amount = atoi(strtok(NULL, " "));
    date = strtok(NULL, "\n");
    addEdge(graph, date, amount, id1, id2, map);
  }
  fclose(file);


//-----------------------------------------------------prompt---------------------------------------------------------

  char* token;
  bool exit = false; 
  do{
    printf("Miris waiting for a command :\n");
    //Διαβάζουμε με getcar γιατί δεν ξέρουμε πόσο μεγάλη θα είναι η είσοδος του χρήστη
    //μπορει να γράψει πολλούς κομβους για insert πχ
    char ch;
    char* command = NULL;
    int size = 0;
    int capacity = 1;
    command = malloc(sizeof(char)* capacity);
    
    while(((ch = getchar()) != '\n') && (ch!= EOF)){
      if(size >= capacity - 1){
        //διπλασιάζουμε το μέγεθος του buffer για να μην κάνουμε πολλά realloc
        capacity *= 2;
        command = realloc(command, capacity * sizeof(char));
      }
      command[size++] = ch;
    }
    command[size] = '\0';
    
    //αν πατηθεί enter χωρίς να γραφτεί τίποτα
    if(strcmp(command, "") == 0){
      free(command);
      continue;
    }

    //αντιγραφή της συμβολοσειράς γιατί θα χρειαστεί για 2η strtok βλ. insert
    char* commandCopy;
    commandCopy = malloc(strlen(command) + 1);
    strcpy(commandCopy, command);

    token = strtok(command, " ");


    //--------------------------------------------------------------- 1 -------------------------------------------------------
    if(strcmp(token, "i") == 0 || strcmp(token, "insert") == 0){

      token = strtok(NULL, " ");
      //εάν το πρώτο όρισμα μετά το insert είναι NULL τότε έχουμε λάθος format
      if(token == NULL){
        printf("   Format error:\n");
        printf("   Command Name : i Ni [Nj Nk ...] or insert Ni [Nj Nk ...]\n\n");
        free(command);
        free(commandCopy);
        continue;
      }

      //έλεγχος αν υπάρχει κάποιος κόμβος με αυτό το id
      bool exists = false;
      while(token != NULL){
        if(mapFind(map, token) != NULL){
          printf("   IssueWith: %s (already exists)\n", token);
          exists = true;
          break;
        }
        token = strtok(NULL, " ");
      }

      if(exists){
        free(command);
        free(commandCopy);
        continue;
      }

      //να ξεπεράσουμε το i της εντολής(χρησιμοποιούμε το αντίγραφο της εντολής 
      //για να είμαστε σίγουροι οτι το περιεχόμενο της είναι ίδιο με την αρχική
      //και να μπορέσουμε χωρίς λάθη να διασχίσουμε την συμβολοσειερά απο την αρχή)

      printf("   Insert into the graph structure 1 or more nodes\n   with specific STRING ids.\n\n");
      token = strtok(commandCopy, " ");
      token = strtok(NULL, " ");

      printf("   Succ: ");
      while(token != NULL){
        printf(" %s", token);
        graphAddNode(graph, token, map);
        token = strtok(NULL, " ");

      }
      printf("\n\n");
      
    }

    //--------------------------------------------------------------- 2 -------------------------------------------------------
    else if(strcmp(token, "n") == 0 || strcmp(token, "insert2") == 0){

      token = strtok(NULL, " ");
      char* token2 = strtok(NULL, " ");
      char* sum = strtok(NULL, " ");
      char* date = strtok(NULL, " ");
      //μετά την ημερομηνία δδεν πρέπει να υπάρχει κάτι άλλο
      char* next = strtok(NULL, " ");

      if(token == NULL || token2 == NULL || sum == NULL || date == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name : n Ni Nj amount date or insert2 n Ni Nj sum date\n\n");
      }
      else{
        printf("   introduce an edge with direction from %s to %s with label\n   %s + %s if either %s or %s does not exist in the graph,\n   do the appropriate node insertion first.\n\n", token, token2, sum, date, token, token2);
        addEdge(graph, date, atoi(sum), token, token2, map);
      }
    }


    //--------------------------------------------------------------- 3 --------------------------------------------------------
    else if(strcmp(token,"d") == 0 || strcmp(token, "delete") == 0){
      token = strtok(NULL, " ");
      //εάν το πρώτο όρισμα μετά το insert είναι NULL τότε έχουμε λάθος format
      if(token == NULL){
        printf("   Format error:\n");
        printf("   Command Name : d Ni [Nj Nk ...] or delete Ni [Nj Nk ...]\n\n");
        free(command);
        free(commandCopy);
        continue;
      }
      printf("   delete from graph listed nodes Ni, Nj, Nk, etc\n\n");

      //έλεγχος αν υπάρχει κάποιος κόμβος με αυτό το id
      bool exists = false;
      while(token != NULL){
        if(mapFind(map, token) == NULL){
          printf("   IssueWith: %s (no exists)\n", token);
          exists = true;
          break;
        }
        token = strtok(NULL, " ");
      }

      if(exists){
        free(command);
        free(commandCopy);
        continue;
      }

      //όμοια με insert
      token = strtok(commandCopy, " ");
      token = strtok(NULL, " ");

      while(token != NULL){
        removeGraphNode(token, map, graph);
        token = strtok(NULL, " ");

      }
      printf("\n\n");

    }

    //--------------------------------------------------------------- 4 --------------------------------------------------------
    else if(strcmp(token, "l" ) == 0 || strcmp(token, "delete2") == 0){

      token = strtok(NULL, " ");
      char* token2 = strtok(NULL, " ");
      //μετά το δεύτερο όρισμα δεν πρέπει να υπάρχει κάτι άλλο
      char* next = strtok(NULL, " ");

      if(next != NULL || token == NULL || token2 == NULL){
        printf("   Format error:\n");
        printf("   Command Name : l Ni Nj or delete2 Ni Nj\n\n");
      }

      else if(findEdge(token, token2, map) == NULL){
        printf("   Edge between %s - %s not found\n\n", token, token2);
      }

      else{
        printf("   remove the edge between %s and %s; if there are\n   more than one edges, remove one of the edges.\n\n", token, token2);
        removeEdge(token, token2, map);
      }
    }


    //--------------------------------------------------------------- 5 --------------------------------------------------------
    else if(strcmp(token, "m") == 0 || strcmp(token, "modify") == 0){
      char* id1  = strtok(NULL, " ");
      char* id2 = strtok(NULL, " ");
      char* sum = strtok(NULL, " ");
      char* sum2 = strtok(NULL, " ");
      char* date = strtok(NULL, " ");
      char* date2 = strtok(NULL, " ");
      char* next = strtok(NULL, " ");

      if(id1 == NULL || id2 == NULL || sum == NULL || sum2 == NULL || date2 == NULL || date == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name :m Ni Nj sum sum1 date date1 or modify Ni Nj sum sum1 date date1\n\n");
      }
      else if(modifyEdge(id1, id2, date, atoi(sum), date2, atoi(sum2), map) == 1){
        printf("   Non-existing edge:\n\n" );
      }
      else{
        printf("   update the values of a specific edge between %s and %s\n\n", id1, id2);
      }
    }

    //--------------------------------------------------------------- 6 --------------------------------------------------------

    else if(strcmp(token, "f") ==0 || strcmp(token, "find") == 0){
      token = strtok(NULL, " ");
      char* next = strtok(NULL, " ");

      if(token == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name : f Ni or find Ni\n\n");
      }

      else if(mapFind(map, token) == NULL){
        printf("   Non-existing node %s \n\n", token);
      }

      else{
        printf("   find all outgoing edges from %s\n\n", token);
        displayOutgoingEdges(token, map);
      }

    }

    //--------------------------------------------------------------- 7 --------------------------------------------------------

    else if(strcmp(token, "r") == 0 || strcmp(token, "receiving")==0){
      token = strtok(NULL, " ");
      char* next = strtok(NULL, " ");

      if(token == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name : r Ni or receiving Ni\n\n");
      }

      else if(mapFind(map, token) == NULL){
        printf("   Non-existing node %s \n\n", token);
      }

      else{
        printf("   find all ingoing edges from %s\n\n", token);
        displayIncomingEdges(token, map);
      }      
    }

    //--------------------------------------------------------------- 8 --------------------------------------------------------
    else if(strcmp(token, "c") == 0 || strcmp(token, "circlefind") == 0){
      token = strtok(NULL, " ");
      char* next = strtok(NULL, " ");

      if(token == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name : c Ni or circlefind Ni\n\n");
      }

      else if(mapFind(map, token) == NULL){
        printf("   Non-existing node %s \n\n", token);
      }

      else{
        printf("   find all circles that contain node %s\n\n", token);
        //Απλους κύκλους χωρίς ελάχιστο ποσό
        findCircles(token, graph, map, 0, 0);
        
      }
    }

    //--------------------------------------------------------------- 9 --------------------------------------------------------
    
    else if(strcmp(token, "fi") == 0 || strcmp(token, "findcircles") == 0){
      token = strtok(NULL, " ");
      char* sum = strtok(NULL, " ");
      char* next = strtok(NULL, " ");

      if(token == NULL || sum == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name : c Ni k or circlefind Ni k\n\n");
      }

      else if(mapFind(map, token) == NULL){
        printf("   Non-existing node %s \n\n", token);
      }

      else{
        printf("   find circular relationships in which %s is involved and moves at least k units of funds.\n", token);
        findCircles(token, graph, map, atoi(sum), 1);
      }
    }


    //--------------------------------------------------------------- 11 --------------------------------------------------------
    else if(strcmp(token, "o") == 0 || strcmp(token, "connected") == 0){
      token = strtok(NULL, " ");
      char* id2 = strtok(NULL, " ");
      char* next = strtok(NULL, " ");

      if(token == NULL || id2 == NULL || next != NULL){
        printf("   Format error:\n");
        printf("   Command Name : o Ni Nj or connected Ni Nj\n\n");
      }

      else if(mapFind(map, token) == NULL){
        printf("   Non-existing node %s \n\n", token);
      }

      else if(mapFind(map, id2) == NULL){
        printf("   Non-existing node %s \n\n", id2);
      }

      else{
        findPath(graph, token, id2, map);
      }
    }


    //--------------------------------------------------------------- 12 --------------------------------------------------------
    else if(strcmp(token, "e") == 0 || strcmp(token, "exit") == 0){
      printf("terminate the program.\n");
      //%zu για να τυπωσουμε size_t
      printf("%zu Bytes released\n\n", currentMemoryUsage);
      exit = true;
    }
    else{
      printf("Unrecognized command: %s\n",token);
    }

    free(command);
    free(commandCopy);
  }
  while(!exit);


  file = fopen(outputFile, "w");
  //TODO MAKE THE OUTPUT FILE IF NOT EXISTS AND IF NOT GIVEN
  if(file == NULL){
    fprintf(stderr, "Error opening file\n");
    return 1;
  }
  printToFile(graph, file);
  fclose(file); 
  destroyGraph(graph);
  mapDestroy(map);

  return 0;
}