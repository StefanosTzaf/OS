#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Map.h"
#include <fcntl.h>

int splitterHashFunction(char *word, int numberOfBuilders);
int countDigits(int number);
char* printingFdsToString(int numOfBuilders, int pipesSplitterToBuilder[][2]);

int* writeFdsToInt(char* pipeWriteEnds, int numOfBuilders);
Map exclusionHashTable(char* fileName);
int compareWords(Pointer a, Pointer b);
void rootReadFromPipe(int readEnd);