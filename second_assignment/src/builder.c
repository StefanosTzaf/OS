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

   int fd = atoi(argv[1]);
   char buffer[4096];

   int sizeofWord = 0;
   int capacity = 10;
   char* word = malloc(capacity);
   while (1) {

      int bytes = read(fd, buffer, sizeof(buffer));


      if (bytes == 0) {
         break;
      }

      for(int i = 0; i < bytes; i++){
         sizeofWord++;

         if (sizeofWord > capacity){
            capacity *= 2;
            word = realloc(word, capacity);
         }


         if(buffer[i] == '-' && sizeofWord > 0){
            //printf("%s\n",word);
            memset(word, '\0', sizeofWord);
            sizeofWord = 0;
         }
         else{
            word[sizeofWord - 1] = buffer[i];
         }

      }

   }
   free(word);
   close(fd);
   
   exit(0);
}

