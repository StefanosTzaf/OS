#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>

int main(int argc, char* argv[]){
    shareDataSegment* sharedData = attachShm();
    size_t sharedMemorySize = sizeof(shareDataSegment);


    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);
}