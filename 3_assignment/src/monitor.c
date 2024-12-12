#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    int shmFd;
    size_t sharedMemorySize = sizeof(shareDataSegment);
    shareDataSegment* sharedData;

    //not creating shared memory, just opening it if initializer has already run
    //not writing to shared memory, just reading from it
    shmFd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shared memory open failed");
        exit(EXIT_FAILURE);
    }

    if (ftruncate(shmFd, sharedMemorySize) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    sharedData = mmap(0, sharedMemorySize, PROT_READ , MAP_SHARED, shmFd, 0);

    if (sharedData == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
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
    return 0;
}