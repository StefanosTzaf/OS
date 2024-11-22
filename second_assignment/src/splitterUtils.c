#include "splitterUtils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int splitterHashFunction(char *word, int numberOfBuilders){
	unsigned int hash = 5381;
	for (char* s = word; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;
	return (hash % numberOfBuilders);

}

int* writeFdsToInt(char* pipeWriteEnds, int numOfBuilders){
	int* fds = malloc(numOfBuilders * sizeof(int));
	char* token = strtok(pipeWriteEnds, " ");
	int i = 0;
	
	while(token != NULL){
		fds[i] = atoi(token);
		token = strtok(NULL, " ");
		i++;
	}
	return fds;
}


Map exclusionHashTable(char* fileName){
	char buffer[1024];
    int bytesRead;
    int lineCount = 0;
	int fd = open(fileName, O_RDONLY);
	if(fd == -1){
		perror("Error opening file");
		exit(1);
	}
    // Read the file in chunks
    while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
        for (int i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                lineCount++;
            }
        }
    }

	Map exclusionMap = mapCreate(splitterCompareWords, free, lineCount);

	lseek(fd, 0, SEEK_SET);
	char ch;
	int sizeOfWord = 0;
	int capacity = 10;
	char* word = malloc(capacity);
	while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0){
		for(int i = 0; i < bytesRead; i++){
			ch = buffer[i];
			if(sizeOfWord == capacity){
				capacity *= 2;
				word = realloc(word, capacity);
			}
			if(ch == '\n'){
				char* newWord = malloc(sizeOfWord);
				strcpy(newWord, word);
				mapInsert(exclusionMap, newWord, newWord);
				memset(word, '\0', sizeOfWord);
				sizeOfWord = 0;
			}
			else{
				word[sizeOfWord] = ch;
				sizeOfWord++;
			}
		}
	}
	if(sizeOfWord > 0){
		char* lastWord = malloc(sizeOfWord);
		strcpy(lastWord, word);
		mapInsert(exclusionMap, lastWord, lastWord);
	}
	close(fd);
	return exclusionMap;

}

int splitterCompareWords(Pointer a, Pointer b){
	MapNode nodeB = (MapNode)b;
	return strcmp(a, mapNodeKey(nodeB));
}
