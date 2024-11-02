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
    printf("pid : %d\n", getpid());
        char buffer[256];

    while(1){
        ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
        if(bytesRead <= 0){
            break;
        }
        write(STDOUT_FILENO, buffer, bytesRead);
    }
    // Optional: if any processing or cleanup is required after reading all lines
    fprintf(stderr, "Splitter process complete.\n");

    return 0;

}