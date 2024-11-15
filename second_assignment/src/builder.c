#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
   if(argc != 2){
      fprintf(stderr, "Usage: ./builder <indexOfBuilder>\n");
      exit(1);
   }
   int fd = atoi(argv[1]);
   char buffer[1024];
   
   // while (1) {
   //    int bytes = read(fd, buffer, sizeof(buffer));
   //    if (bytes == 0) {
   //       break;
   //    }
   //    printf("%s", buffer);
   // }

   exit(0);
}