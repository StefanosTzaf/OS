#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
   if(argc != 2){
      fprintf(stderr, "Usage: ./builder <read end fd for pipe>\n");
      exit(1);
   }
   int fd = atoi(argv[1]);
   char buffer[1024];

   int sizeofWord = 0;
   char* word = NULL;
   while (1) {
      int bytes = read(fd, buffer, sizeof(buffer));
      if (bytes == 0) {
         break;
      }
     //printf ("%s\n", buffer);
      for(int i = 0; i < bytes; i++){
         sizeofWord++;
         word = realloc(word, sizeofWord);

         // if(buffer[i] == ' ' && sizeofWord > 0){
         //    word[sizeofWord - 1] = '\0';
         //    printf("%s\n", word);
         //    free(word);
         //    word = NULL;
         //    sizeofWord = 0;
         // }
         // else{
         //    word[sizeofWord - 1] = buffer[i];

         // } 

      }

   }

   close(fd);
   
   exit(0);
}