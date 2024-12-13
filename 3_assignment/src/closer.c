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
    
 
 
    shm_unlink(sharedMemoryName);

    exit(EXIT_SUCCESS);

}