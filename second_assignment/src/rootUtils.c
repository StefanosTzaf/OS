#include "rootUtils.h"

struct wordsInRoot{
	char* word;
	int frequency;
};


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
	free(word);
	free(frequency);
	return set;
}


void printingTopK(Set set, int k){
	//the set is sorted in ascending order so the last k nodes are the k most frequent words
	SetNode root = getRootNode(set);
	SetNode node = setLast(set);
	
	for(int i = 0; i < k; i++){
		char counter[16] ;
		sprintf(counter, "%d", i + 1);
		struct wordsInRoot* wordInRoot = (struct wordsInRoot*)setNodeValue(set, node);
		printf("%s: {%s, %d}\n",counter, wordInRoot->word, wordInRoot->frequency);

		node = nodeFindPrevious(root, set, node);
	}
}