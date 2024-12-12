#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    shareDataSegment* sharedData = attachShm();
    size_t sharedMemorySize = sizeof(shareDataSegment);
    
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


    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);
}