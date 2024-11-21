#include "utils.h"

struct wordsInRoot{
	char* word;
	int frequency;
};

int splitterHashFunction(char *word, int numberOfBuilders){
	unsigned int hash = 5381;
	for (char* s = word; *s != '\0'; s++)
		hash = (hash << 5) + hash + *s;
	return (hash % numberOfBuilders);

}

int countDigits(int number) {
    if (number == 0) {
        return 1;
    }

    int count = 0;
    while (number != 0) {
        number /= 10;
        count++;
    }
    return count;
}

char* printingFdsToString(int numOfBuilders, int pipesSplitterToBuilder[][2]){
	char* pipeWriteEnds = malloc(1);
	pipeWriteEnds[0] = '\0';
	for(int i = 0; i < numOfBuilders; i++){
		int digits = countDigits(pipesSplitterToBuilder[i][1]);
		//1 position for space and digits for the file descriptor
		pipeWriteEnds = realloc(pipeWriteEnds, digits + 1 + strlen(pipeWriteEnds));
		sprintf(pipeWriteEnds + strlen(pipeWriteEnds), "%d ", pipesSplitterToBuilder[i][1]);
	}
	return pipeWriteEnds;
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

	Map exclusionMap = mapCreate(compareWords, free, lineCount);

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

int compareWords(Pointer a, Pointer b){
	MapNode nodeB = (MapNode)b;
	return strcmp(a, mapNodeKey(nodeB));
}


void rootReadFromPipe(int readEnd){
	char buffer[4096];

	int sizeofWord = 0;
	int capacity = 10;
	char* word = malloc(capacity);

	int sizeOfFrequency = 0;
	int capacityFrequency = 10;
	char* frequency = malloc(capacityFrequency);


	while (1) {
		//reading from pipe until there is no more data
		int bytes = read(readEnd, buffer, sizeof(buffer));
		if (bytes <= 0) {
			break;
		}
		printf("%s\n", buffer);

		for(int i = 0; i < bytes; i++){
			
			//creating the word
			if(isalpha(buffer[i])){
				sizeofWord++;
				if (sizeofWord > capacity){
					capacity *= 2;
					word = realloc(word, capacity);
				}
			
			}
			//copying the word
			else if( buffer[i] == '*'){
				continue;
			}
			//creating the frequency 
			else if(isdigit(buffer[i])){
				sizeOfFrequency++;
				if (sizeOfFrequency > capacityFrequency){
					capacityFrequency *= 2;
					frequency = realloc(frequency, capacityFrequency);
				}
				frequency[sizeOfFrequency - 1] = buffer[i];
			
			
			}
			//copying the frequency and the word to the struct
			else if(buffer[i] == '-' ){
				struct wordsInRoot* wordInRoot = malloc(sizeof(struct wordsInRoot));
				wordInRoot->word = malloc(sizeofWord + 1);
				strcpy(wordInRoot->word, word);
				wordInRoot->word[sizeofWord] = '\0';
				wordInRoot->frequency = atoi(frequency);
				//printf("Word: %s Frequency: %d\n", wordInRoot->word, wordInRoot->frequency);

			}
			//ready for the next word
			else if(buffer[i] == '\0'){
				sizeofWord = 0;
				sizeOfFrequency = 0;
				memset(word, '\0', sizeofWord);
				memset(frequency, '\0', sizeOfFrequency);

			}

		}

	}

	free(word);
}