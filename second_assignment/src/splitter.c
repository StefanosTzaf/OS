#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

int main(int argc, char* argv[]){
    // without arguments (the file is open from the exec call, we do not pass it as an argument)
    if(argc != 5){
        fprintf(stderr, "Usage: ./splitter <inputFile> <startLine> <endLine> <numberOfBytes>\n");
        exit(1);
    }
    int fd = open(argv[1], O_RDONLY);
    if(fd == -1){
        perror("Error opening file");
        exit(1);
    }
    // read the file from where this specific splitter starts
    int startLine = atoi(argv[2]);
    int endLine = atoi(argv[3]);
    int firstByteToRead = atoi(argv[4]);

    lseek(fd, firstByteToRead, SEEK_SET);
    printf("Splitter id : %d\n", getpid());

    int linesToRead = endLine - startLine + 1;

    int currentLine = startLine;
    char buffer[1024];
    bool allLinesRead = false;
    
    int bytesRead = read(fd, buffer, sizeof(buffer));

    while(bytesRead > 0){

        int size = 0;
        // if a word ends with punctuation
        int endsWithPunct = 0;
        int capacity = 10;
        // if a word is of the form λ3ξη$ then it is not valid
        int valid = 1;
        int ch;
        char *word = malloc(capacity * sizeof(char));

        for (int i = 0; i < bytesRead; i++) {
            char ch = buffer[i];
            if (ch == '\n') {
                currentLine++;
            }
            // if we reached the next line, after the last one, the splitter's job is done
            if(currentLine == atoi(argv[3]) + 1){
                allLinesRead = true;
                break;
            }

            if (isalpha(ch)) {
                // dynamic increase of word (we have no limit on the word size)
                if (size >= capacity - 1) {
                    capacity *= 2;
                    word = realloc(word, capacity * sizeof(char));
                }
                word[size++] = ch;
                endsWithPunct = 0;
                valid = 1;
                continue;
            } 
            // else if ((ch == '!' || ch == '?' || ch == '.') && (size > 0) && (valid)) {
            //     endsWithPunct = 1;
            //     word[size] = '\0';
            //     printf("Read valid word: %s\n", word);
            //     size = 0;
            // } 
            else if (isspace(ch) ) {
                if (size > 0 && valid) {
                    if (size > 0) {
                        word[size] = '\0';
                        printf("Read valid word: %s\n", word);
                    }
                }
                // for the next word
                size = 0;
                valid = 1;
                endsWithPunct = 0;
            } 
            // else {
            //     // in case we have an invalid character
            //     valid = 0;
            //     // skip characters until we find a space
            //     while ((ch != EOF) && (ch != ' ' ) && (ch != '\n')) {
            //         printf("Skipping invalid character: %c\n", ch);
            //         ch = getchar();
            //     }
            //     size = 0;

            // }
            
        }
        // Handle the last word if input didn't end with a space
        // if (size > 0 && valid) {
        //     if (endsWithPunct || size > 0) {
        //         word[size] = '\0';
        //         printf("Read valid word: %s\n", word);
        //     }
            

        free(word);


        if(allLinesRead){
            break;
        }
        // if we have read the entire buffer and have not read
        // the lines corresponding to the splitter, then read again

        if(bytesRead == 0){
            bytesRead = read(fd, buffer, sizeof(buffer));
        }

    }

    close(fd);
    return 0;

}