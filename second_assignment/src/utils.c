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
		pipeWriteEnds = realloc(pipeWriteEnds, digits + 2 + strlen(pipeWriteEnds));
		sprintf(pipeWriteEnds + strlen(pipeWriteEnds), "%d ", pipesSplitterToBuilder[i][1]);
	}
	pipeWriteEnds[strlen(pipeWriteEnds) - 1] = '\0';
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

void destroyMapNode(Pointer node){
	MapNode mapNode = (MapNode)node;
	free(mapNodeKey(mapNode));
	free(mapNodeValue(mapNode));
	free(mapNode);
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


int compareSetNodes(Pointer a, Pointer b){
	struct wordsInRoot* wordInRootA = (struct wordsInRoot*)a;
	struct wordsInRoot* wordInRootB = (struct wordsInRoot*)b;
	//firstly we compare the frequency of the words
	if(wordInRootA->frequency > wordInRootB->frequency){
		return 1;
	}
	else if(wordInRootA->frequency < wordInRootB->frequency){
		return -1;
	}
	else{
		return strcmp(wordInRootA->word, wordInRootB->word);
	}
}

void destroySetNode(Pointer node){
	struct wordsInRoot* wordInRoot = (struct wordsInRoot*)node;
	free(wordInRoot->word);
	free(wordInRoot);
}

Set rootReadFromPipe(int readEnd){
	Set set = setCreate(compareSetNodes, destroySetNode);

	char buffer[4096];

	int sizeOfWord = 0;
	int capacity = 10;
	char* word = malloc(capacity);

	int sizeOfFrequency = 0;
	int capacityFrequency = 10;
	char* frequency = malloc(capacityFrequency);

	while (1) {
		//reading from pipe until there is no more data
		
		memset(buffer, '\0', sizeof(buffer));
		int bytes = read(readEnd, buffer, sizeof(buffer));
		if (bytes <= 0) {
			break;
		}

		for(int i = 0; i < bytes; i++){

			//creating the word
			if(isalpha(buffer[i])){
				sizeOfWord++;
				if (sizeOfWord >= capacity){
					capacity *= 2;
					word = realloc(word, capacity);
				}
				word[sizeOfWord - 1] = buffer[i];
			
			}
			//copying the word
			else if( buffer[i] == '*'){
				continue;
			}
			//creating the frequency 
			else if(isdigit(buffer[i])){
				sizeOfFrequency++;
				if (sizeOfFrequency >= capacityFrequency){
					capacityFrequency *= 2;
					frequency = realloc(frequency, capacityFrequency);
				}
				frequency[sizeOfFrequency - 1] = buffer[i];
			
			}
			//copying the frequency and the word to the struct
			else if(buffer[i] == '-' ){
				//there will be alawys one more position because we realloc when the sizeOfFrequency becomes equal to the size
				frequency[sizeOfFrequency] = '\0';

				struct wordsInRoot* wordInRoot = malloc(sizeof(struct wordsInRoot));
				wordInRoot->word = malloc(sizeOfWord + 1);
				strcpy(wordInRoot->word, word);
				wordInRoot->word[sizeOfWord] = '\0';
				wordInRoot->frequency = atoi(frequency);
				setInsert(set, wordInRoot);
				//ready for the next word
				sizeOfWord = 0;
				sizeOfFrequency = 0;
				memset(word, '\0', capacity);
				memset(frequency, '\0', capacityFrequency);

			}
			else if(buffer[i] == '\0'){
				continue;
			}


		}

	}
	return set;
	free(word);
	free(frequency);
}


void printingTopK(Set set, int k){
	//the set is sorted in ascending order so the last k nodes are the k most frequent words
	SetNode root = getRootNode(set);
	SetNode node = setLast(set);
	
	for(int i = 0; i < k; i++){
		struct wordsInRoot* wordInRoot = (struct wordsInRoot*)setNodeValue(set, node);
		printf("%s %d\n", wordInRoot->word, wordInRoot->frequency);

		node = nodeFindPrevious(root, set, node);
	}
}