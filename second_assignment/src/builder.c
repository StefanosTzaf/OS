#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "builder.h"


int main(int argc, char* argv[]){
   if(argc != 2){
      fprintf(stderr, "Usage: ./builder <read end fd for pipe>\n");
      exit(1);
   }

   Map wordHashTable = mapCreate(compareWords, free, 1000);

   int fd = atoi(argv[1]);
   char buffer[4096];

   int sizeofWord = 0;
   int capacity = 10;
   char* word = malloc(capacity);
   while (1) {

      int bytes = read(fd, buffer, sizeof(buffer));
      if (bytes <= 0) {
         break;
      }

      for(int i = 0; i < bytes; i++){
         sizeofWord++;

         if (sizeofWord > capacity){
            capacity *= 2;
            word = realloc(word, capacity);
         }


         if(buffer[i] == '-' && sizeofWord > 0){
            //if the word arrives first time in the builder add it to  the hash table
            if(mapFind(wordHashTable, word) == NULL){
               char* newWord = malloc(sizeofWord);
               int* frequency = malloc(sizeof(int));
               *frequency = 1;
               strcpy(newWord, word);
               mapInsert(wordHashTable, newWord, frequency);
            }
            //else increment the frequency of the word
            else{
               int* frequency = mapFind(wordHashTable, word);
               (*frequency)++;
            }
            memset(word, '\0', sizeofWord);
            sizeofWord = 0;
         }
         else{
            word[sizeofWord - 1] = buffer[i];
         }

      }

   }
   free(word);
   

   for(MapNode node = mapFirst(wordHashTable); node != NULL; node = mapGetNext(wordHashTable, node)){
      int frequency;
      char wordToPrint[strlen(mapNodeKey(node))];

      strcpy(wordToPrint, mapNodeKey(node));
      frequency = *(int*)mapNodeValue(node);
      //printf("%s %d\n", wordToPrint, frequency);
   }



   mapDestroy(wordHashTable);
   close(fd);
   exit(0);
}

