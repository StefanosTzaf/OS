#include "utils.h"
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void initializeSharedValues(shareDataSegment *sharedData) {
    sharedData->sharedStatistics.averageWaitingTime = 0.0;
    sharedData->sharedStatistics.totalWaitingTime = 0.0;
    sharedData->sharedStatistics.averageStayTime = 0.0;
    sharedData->sharedStatistics.totalStayTime = 0.0;
    sharedData->sharedStatistics.consumedWine = 0;
    sharedData->sharedStatistics.consumedWater = 0;
    sharedData->sharedStatistics.consumedCheese = 0;
    sharedData->sharedStatistics.consumedSalads = 0;
    sharedData->sharedStatistics.visitorsServed = 0;

    //initialize the FCFS waiting circular buffer
    sharedData->fcfsBuffer.front = 0;
    sharedData->fcfsBuffer.back = 0;
    //every semaphore in buffer is initialized to 1 while no one has already taken the position in buffer
    for (int i = 0; i < MAX_VISITORS; i++) {
        sem_init(& (sharedData->fcfsBuffer.positionSem[i]), 1, 1);
        sharedData->fcfsBuffer.buffer[i] = -1;
    }

    
    sem_init(&sharedData->exceedingVisitorsSem, 1, MAX_VISITORS);

    //Initialize the order circular buffer
    sharedData->orderBuffer.front = 0;
    sharedData->orderBuffer.back = 0;
    for (int i = 0; i < 12; i++) {
        sem_init(&sharedData->orderBuffer.chairSem[i], 1, 1);
        sharedData->orderBuffer.orderCircularBuffer[i].visitor = -1;
        sharedData->orderBuffer.orderCircularBuffer[i].water = false;
        sharedData->orderBuffer.orderCircularBuffer[i].wine = false;
        sharedData->orderBuffer.orderCircularBuffer[i].cheese = false;
        sharedData->orderBuffer.orderCircularBuffer[i].salad = false;
    }

    // Initialize the tables
    for (int i = 0; i < 3; i++) {
        sharedData->tables[i].isOccupied = false; // All tables are unoccupied initially
        sharedData->tables[i].chairsOccupied = 0;
        for(int j = 0; j < 4; j++) {
            sharedData->tables[i].chairs[j] = -1; // No visitor is sitting on any chair initially
        }
    }


    // Initialize the mutex and receptionist semaphore
    sem_init(&sharedData->mutex, 1, 1);
    sem_init(&sharedData->receptionistSem, 1, 0); // Receptionist will be asleep until there is work

    // Set the closing flag to false initially
    sharedData->closingFlag = false;
}


//function to attch shared memory (used in receptionist and visitor and monitor processes)
//they not create shared memory, they just attach it
shareDataSegment* attachShm(void){
    int shmFd;
    size_t sharedMemorySize = sizeof(shareDataSegment);
    shareDataSegment* sharedData;

    //open shared memory
    shmFd = shm_open(SHARED_MEMORY_NAME, O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shared memory open failed");
        exit(EXIT_FAILURE);
    }

    //define the size
    if (ftruncate(shmFd, sharedMemorySize) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    //Map shared memory in current address space
    sharedData = mmap(0, sharedMemorySize, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (sharedData == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }
    return sharedData;
}