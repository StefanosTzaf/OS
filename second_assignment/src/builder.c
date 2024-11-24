#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "Map.h"
#include "builderUtils.h"   
#include <signal.h>

int main(int argc, char* argv[]){
   if(argc != 4){
      fprintf(stderr, "Usage: ./builder, <readEndForBuilder>, <writeEndForBuilder>, <totalWordsToRead>\n");
      exit(1);
   }

   int readEndFd = atoi(argv[1]);
   int writeEndFd = atoi(argv[2]);
   int hashSize = atoi(argv[3]);
   char buffer[4096];

   Map wordHashTable = mapCreate(builderCompareWords, destroyMapNode, hashSize);

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
   //parent process pid
   pid_t rootPid = getppid();
   //sending singal to root
   kill(rootPid, SIGUSR2);
   
   exit(0);
}
