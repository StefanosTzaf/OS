#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "utils.h"


int main(int argc, char* argv[]){
    shareDataSegment* sharedData = attachShm();
    size_t sharedMemorySize = sizeof(shareDataSegment);

    sem_wait(&(sharedData->receptionistSem));
    sem_wait(&(sharedData->mutex));

    sem_post(&(sharedData->mutex));




    munmap(sharedData, sharedMemorySize);

    exit(EXIT_SUCCESS);
}