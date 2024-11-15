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
   
   while (1) {
      int bytes = read(fd, buffer, sizeof(buffer));
      printf("Builder %d received: %s\n", fd, buffer);
      if (bytes == 0) {
         break;
      }
   }



   close(fd);
   
   exit(0);
}