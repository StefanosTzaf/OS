#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(){
    int fd = open("input.txt", O_RDONLY);
    if(fd == -1){
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    int lines = 1;
    int bytesRead;
    char buffer[10];
    read(fd, buffer, 10);
    
    printf("%s\n", buffer);
    lseek(fd, 15, SEEK_SET);
    read(fd, buffer, 1);
    printf("%s\n", buffer);
}