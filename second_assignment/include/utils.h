#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int splitterHashFunction(char *word, int numberOfBuilders);
int countDigits(int number);
char* printingFdsToString(int numOfBuilders, int pipesSplitterToBuilder[][2]);

int* writeFdsToInt(char* pipeWriteEnds, int numOfBuilders);