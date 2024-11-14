#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "utils.h"

int main(int argc, char* argv[]){
    // without arguments (the file is open from the exec call, we do not pass it as an argument)
    if(argc != 6){
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
    int numberOfBuilders = atoi(argv[5]);

    // set the read pointer to the first byte of the splitter
    lseek(fd, firstByteToRead, SEEK_SET);
    int linesToRead = endLine - startLine + 1;
    int currentLine = startLine;

    write(STDOUT_FILENO, &linesToRead, sizeof(int));

    char buffer[1024];
    bool allLinesRead = false;
    int bytesRead = read(fd, buffer, sizeof(buffer));
    while(bytesRead > 0){

        int size = 0;
        //the initial capacity for the word
        int capacity = 10;
        // if a word is of the form w#ord then it is not valid
        int valid = 1;
        char *word = malloc(capacity * sizeof(char));

        //----------------------------------------------- words' seperation and checks -----------------------------------------------
        int i = 0; 
        while(i < bytesRead){
            char ch = buffer[i];
            if (ch == '\n') {
                currentLine++;
            }
            // if we reached the next line, after the last one, the splitter's job is done
            if(currentLine == endLine + 1){
                allLinesRead = true;
                // Handle the last word
                if(size > 0 && valid){
                    word[size] = '\0';
                    int hash = splitterHashFunction(word, numberOfBuilders);
                }
                break;
            }
            
            // if the character is a letter, add it to the word
            if (isalpha(ch)) {
            // dynamic increase of word (we have no limit on the word size)
                if (size >= capacity - 1) {
                    capacity *= 2;
                    word = realloc(word, capacity * sizeof(char));
                }
                word[size++] = ch;
            } 
            else if (ispunct(ch) && (valid)) {
                //a punctuation character  found in the beginning etc "word

                if(size == 0){
                }
                // a punctuation character found and the word ends 
                else{
                    word[size] = '\0';
                    int hash = splitterHashFunction(word, numberOfBuilders);
                    size = 0;
                }
            } 
            else if (ch == ' ' ||ch == '\n') {
                if (size > 0 && valid) {
                    word[size] = '\0';
                    int hash = splitterHashFunction(word, numberOfBuilders);
                }
                // for the next word
                size = 0;
                valid = 1;
            } 
            else {
                // in case we have an invalid character
                valid = 0;
                // skip characters until we find a space
                while ((ch != ' ' ) && (ch != '\n')) {
                    i++;
                    ch = buffer[i];
                }
                

            }
            if(valid){
                //if the character was not invalid, we move to the next character
                i++;
            }
        }

        
        //--------------------------------------------------------------------------------------------------
        if(size > 0 && valid){
            word[size] = '\0';
            int hash = splitterHashFunction(word, numberOfBuilders);
        }
        free(word);
        if(allLinesRead){
            break;
        }
        // if we have read the entire buffer and have not read
        // the lines corresponding to the splitter, then read again

        bytesRead = read(fd, buffer, sizeof(buffer));
    

    }


    printf("Splitter %d done\n", getpid());
    close(fd);
    return 0;

}