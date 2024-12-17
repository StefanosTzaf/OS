#include <semaphore.h>
#include "utils.h"
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]){

    if(argc != 3){
        fprintf(stderr, "Usage: ./closer -s sharedMemoryName\n");
        exit(EXIT_FAILURE);
    }
    int option;
    char sharedMemoryName[64];

    while((option = getopt(argc, argv, "s:")) != -1){
        if(option == 's'){
            snprintf(sharedMemoryName, sizeof(sharedMemoryName), "/%s", optarg);
        }
        else{
            fprintf(stderr, "Usage: ./closer -s sharedMemoryName\n");
            exit(EXIT_FAILURE);
        }
    }

    shareDataSegment* sharedData = attachShm(sharedMemoryName);
    size_t sharedMemorySize = sizeof(shareDataSegment);

    munmap(sharedData, sharedMemorySize);
 
    sharedData->closingFlag = true;

    
    //TODO: WAITING FOR VISITORS TO LEAVE


    //TODO: PRINTING STATS
    
    //destroying semaphores
    for(int i = 0; i < MAX_VISITORS; i++){
        sem_destroy(&(sharedData->fcfsWaitingBuffer.positionSem[i]));
    }
    sem_destroy(&(sharedData->exceedingVisitorsSem));
    for(int i = 0; i < 12; i++){
        sem_destroy(&(sharedData->orderBuffer.chairSem[i]));
    }
    sem_destroy(&(sharedData->mutex));
    sem_destroy(&(sharedData->receptionistSem));

    //destroying shared memory
    shm_unlink(sharedMemoryName);

    exit(EXIT_SUCCESS);

}