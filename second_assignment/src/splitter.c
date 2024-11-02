#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char* argv[]){
    //χωρίς ορίσματα(το αρχείο είναι ανοιχτό από τη κλήση της exec δεν το περνάμε σαν όρισμα)
    if(argc != 1){
        fprintf(stderr, "Usage: ./splitter <startLine> <endLine> <file descriptor>\n");
        return 1;
    }
    
    printf("Splitter started %d \n", getpid());
    return 0;

}