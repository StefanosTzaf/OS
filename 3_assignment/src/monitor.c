#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>



int main(int argc, char* argv[]){
    if(argc != 3){
        fprintf(stderr, "Usage: ./monitor -s sharedMemoryName\n");
        exit(EXIT_FAILURE);
    }
    int option;
    char sharedMemoryName[64];
    if ((option = getopt(argc, argv, "s:")) != -1) {
        if(option == 's'){
            snprintf(sharedMemoryName, sizeof(sharedMemoryName), "/%s", optarg);
        }
        else{
            fprintf(stderr, "Usage: ./monitor -s sharedMemoryName\n");
            exit(EXIT_FAILURE);
        }
    }

    shareDataSegment* sharedData = attachShm(sharedMemoryName);
    size_t sharedMemorySize = sizeof(shareDataSegment);
    
    sem_wait(&(sharedData->mutex));

    printf("\n");

    for(int i = 0; i < 3; i++) {
        printf("Table %d -> ", i);
        if(sharedData->tables[i].isOccupied) {
            printf("Occupied\n");
        }
        else{
            printf("Unoccupied\n");
        }
        printf("Chairs occupied on this table: %d\n\n", sharedData->tables[i].chairsOccupied);
    }

    printf("            Total products consumed by %d visitors:\n\n",sharedData->sharedStatistics.visitorsServed); 

    printf("Water:  %d\n", sharedData->sharedStatistics.consumedWater);
    printf("Wine:   %d\n", sharedData->sharedStatistics.consumedWine);
    printf("Cheese: %d\n", sharedData->sharedStatistics.consumedCheese);
    printf("Salads: %d\n\n", sharedData->sharedStatistics.consumedSalads);

    sem_post(&(sharedData->mutex));
    
    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);
}