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
   if(argc != 3){
      fprintf(stderr, "Usage: ./builder <read end fd for pipe>\n");
      exit(1);
   }

   Map wordHashTable = mapCreate(compareWords, destroyMapNode, 10000);

   int readEndFd = atoi(argv[1]);
   int writeEndFd = atoi(argv[2]);
   char buffer[4096];

   int sizeofWord = 0;
   int capacity = 10;
   char* word = malloc(capacity);

   while (1) {

      int bytes = read(readEndFd, buffer, sizeof(buffer));

      if (bytes <= 0) {
         break;
      }

      for(int i = 0; i < bytes; i++){


         if(buffer[i] == '-' && sizeofWord > 0){
            word[sizeofWord] = '\0';
            //if the word arrives first time in the builder add it to  the hash table
            if(mapFind(wordHashTable, word) == NULL){
               char* newWord = malloc(sizeofWord + 1);
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
            sizeofWord++;

            if (sizeofWord >= capacity){
               capacity *= 2;
               word = realloc(word, capacity);
            }

            word[sizeofWord - 1] = buffer[i];
         }

      }

   }


   free(word);
   close(readEndFd);


   for(MapNode node = mapFirst(wordHashTable); node != NULL; node = mapGetNext(wordHashTable, node)){
      int sizeOfKey = strlen(mapNodeKey(node));
      char wordToPrint[sizeOfKey];
      strcpy(wordToPrint, mapNodeKey(node));

      int frequency;
      frequency = *(int*)mapNodeValue(node);
      //convert frequency to string
      char frequencyStr[countDigits(frequency)];
      sprintf(frequencyStr, "%d", frequency);
      // printf("Word: %s %d, Frequency: %d \n", wordToPrint,sizeOfKey, frequency);
      //a buffer to store a word like this: word*5-  (5 is the frequency of the word "*" and "-" to seperate frequency and real words)
      int sizeOfBuffer = sizeOfKey + 3 + countDigits(frequency);

      char bufferToWrite[sizeOfBuffer];

      strcpy(bufferToWrite, wordToPrint);
      bufferToWrite[sizeOfKey] = '*';
      strcpy(bufferToWrite + sizeOfKey + 1, frequencyStr);
      bufferToWrite[sizeOfBuffer - 2] = '-';
      bufferToWrite[sizeOfBuffer - 1] = '\0';

      write(writeEndFd, bufferToWrite, strlen(bufferToWrite));


   }


   mapDestroy(wordHashTable);
   close(writeEndFd);
   exit(0);
}

