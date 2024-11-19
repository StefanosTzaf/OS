#include <stdio.h>
#include <stdlib.h>

int global_var = 0;   // Initialized data segment

int main() {
    int local_var = 0;  // Stack segment
    char *heap_var = malloc(sizeof(char));  // Heap segment
    printf("Text segment address: %p\n", (void*)&main);  // Address of code (text segment)
    printf("Initialized data address: %p\n", (void*)&global_var);  // Address of initialized data
    printf("Stack address: %p\n", (void*)&local_var);  // Address of stack
    printf("Heap address: %p\n", (void*)heap_var);  // Address of heap
    free(heap_var);  // Free allocated memory
    return 0;
}