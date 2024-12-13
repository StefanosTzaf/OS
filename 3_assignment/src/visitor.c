#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>


int main(int argc, char* argv[]){

    if(argc != 5){
        fprintf(stderr, "Usage: ./visitor -d <restTime> -s sharedMemoryName\n");
        exit(EXIT_FAILURE);
    }
    int option;
    int maxOrderTime;
    char sharedMemoryName[64];

    while((option = getopt(argc, argv, "d:s:")) != -1){
        if(option == 'd'){
            maxOrderTime = atoi(optarg);
        }
        else if(option == 's'){
            snprintf(sharedMemoryName, sizeof(sharedMemoryName), "/%s", optarg);
        }
        else{
            fprintf(stderr, "Usage: ./visitor -d <restTime> -s sharedMemoryName\n");
            exit(EXIT_FAILURE);
        }
    }

    shareDataSegment* sharedData = attachShm(sharedMemoryName);
    size_t sharedMemorySize = sizeof(shareDataSegment);


    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);
}