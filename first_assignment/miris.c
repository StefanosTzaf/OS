#include "Graph.h"
#include <getopt.h>


int main(int argc, char *argv[]){


  int option;

  //Για να αποθηκεύσουμε το όνομα του αρχείου εισόδου και εξόδου από την γραμμή εντολών
  char* inputFile = NULL;
  char* outputFile = NULL;

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

  char line[256];//για το διαβασμα μίας γραμμής του αρχείου
  int counter = 0;//Ποσες γραμμες έχει το αρχείο για το μέγεθος του Hash table
  while (fgets(line, sizeof(line), file) != NULL) {
      counter++;  // Increment the line count for each line read
  }
  printf("The file has %d lines.\n", counter);
  Graph graph = graphCreate();
  Map map = mapCreate(compareMapNodes, destroyMapNodes, counter);

  rewind(file);//από την αρχή του αρχείου
  while (fgets(line, sizeof(line), file) != NULL) {
    char* id1;
    char* id2;
    int amount;
    char* date;
    id1 = strtok(line, " ");
    id2 = strtok(NULL, " ");
    amount = atoi(strtok(NULL, " "));
    date = strtok(NULL, " ");
    addVertex(graph, date, amount, id1, id2, map);
  }
  fclose(file);


//-----------------------------------------------------prompt---------------------------------------------------------

  // char command[256];

  // while(1){







    
  // }









  file = fopen(outputFile, "w");
  //TODO MAKE THE OUTPUT FILE IF NOT EXISTS AND IF NOT GIVEN
  // if(file == NULL){
  //   fprintf(stderr, "Error opening file\n");
  //   return 1;
  // }
  printToFile(graph, file);
  fclose(file); 

  destroyGraph(graph);
  mapDestroy(map);

  return 0;
}