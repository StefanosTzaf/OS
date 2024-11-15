#include "utils.h"

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