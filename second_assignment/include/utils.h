#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Map.h"
#include <fcntl.h>
#include <ctype.h>
#include "Set.h"

int splitterHashFunction(char *word, int numberOfBuilders);
int countDigits(int number);
char* printingFdsToString(int numOfBuilders, int pipesSplitterToBuilder[][2]);

int* writeFdsToInt(char* pipeWriteEnds, int numOfBuilders);
Map exclusionHashTable(char* fileName);
int compareWords(Pointer a, Pointer b);
Set rootReadFromPipe(int readEnd);
void destroyMapNode(Pointer node);

int compareSetNodes(Pointer a, Pointer b);

void destroySetNode(Pointer node);

void printingTopK(Set set, int k);