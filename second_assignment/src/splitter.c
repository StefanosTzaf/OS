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
    if(argc != 8){
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
    char* writeEnds = argv[6];
    int fdExclusion = atoi(argv[7]);
    Map exclusionMap = exclusionHashTable(fdExclusion);

    //file descriptors for writing in every pipe
    int* writeEndFds = writeFdsToInt(writeEnds, numberOfBuilders);

    // set the read pointer to the first byte of the splitter
    lseek(fd, firstByteToRead, SEEK_SET);
    int linesToRead = endLine - startLine + 1;
    int currentLine = startLine;


    char buffer[1024];
    bool allLinesRead = false;
    int bytesRead = read(fd, buffer, sizeof(buffer));
    while(bytesRead > 0){

        //the initial capacity for the word
        int size = 0;
        int capacity = 10;
        // if a word is of the form w#ord then it is not valid
        int valid = 1;
        char *word = malloc(capacity * sizeof(char));
        //if the word is the last one

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
                    int hash = splitterHashFunction(word, numberOfBuilders);
                    if(mapFindNode(exclusionMap, word) == NULL){
                        word[size] = '-';
                        write(writeEndFds[hash], word, strlen(word));
                    }
                    write(writeEndFds[hash], word, strlen(word));
                    // for the next word
                    memset(word, '\0', capacity);
                    size = 0;
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

                
                // a punctuation character found and the word ends 
                if(size > 0){
                    int hash = splitterHashFunction(word, numberOfBuilders);
                    if(mapFindNode(exclusionMap, word) == NULL){
                        word[size] = '-';
                        write(writeEndFds[hash], word, strlen(word));
                    }

                    memset(word, '\0', capacity);
                    size = 0;
                }
                //else skip the punctuation character if it is in the beggining of the word
            }

            else if (isspace(ch) && (valid)) {
                if (size > 0 && valid) {
                    int hash = splitterHashFunction(word, numberOfBuilders);
                    if(mapFindNode(exclusionMap, word) == NULL){
                        word[size] = '-';
                        write(writeEndFds[hash], word, strlen(word));
                    }
                    memset(word, '\0', capacity);
                    size = 0;
                    
                }
                // for the next word
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
                size = 0;
            }
            if(valid){
                //if the character was not invalid, we move to the next character
                i++;
            }
        }

        
        //--------------------------------------------------------------------------------------------------
        //if the word was the last of the file
        if(size > 0 && valid){
            int hash = splitterHashFunction(word, numberOfBuilders);
            if(mapFindNode(exclusionMap, word) == NULL){
                word[size] = '-';
                write(writeEndFds[hash], word, strlen(word));
            }
            memset(word, '\0', capacity);
            size = 0;

        }
        
        free(word);
        if(allLinesRead){
            break;
        }
        // if we have read the entire buffer and have not read
        // the lines corresponding to the splitter, then read again

        bytesRead = read(fd, buffer, sizeof(buffer));
    

    }
    

    for(int i = 0; i < numberOfBuilders; i++){
        close(writeEndFds[i]);
    }
    close(fd);
    free(writeEndFds);

    exit(0);

}