#include <stdio.h>
#include <stdlib.h>

int global = 1;   // Initialized data
int main() {
    int local = 0;  // Stack
    char *heap = malloc(sizeof(char));  // Heap
    printf("Text segment address: %p\n", &main);  // Address of code (text segment)
    printf("Initialized data address: %p\n", &global);
    printf("Stack address: %p\n", &local);
    printf("Heap address: %p\n", heap);
    free(heap);
    return 0;
}