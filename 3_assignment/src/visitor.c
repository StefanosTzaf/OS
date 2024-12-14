#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>


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

    // if there is no place for visitor to wait inside the bar they should wait outside.
    // So if in this semaphore P() has been applied more than MAX_VISITORS times then the fcfsWaitingBuffer is full 
    sem_wait(&(sharedData->exceedingVisitorsSem));
    

    if(sharedData->closingFlag){
        munmap(sharedData, sharedMemorySize);
        exit(EXIT_SUCCESS);
    } 

    // if the execution has reached here, there is space in waiting buffer OR yhe waiting buffer is absolutely free
    // and the bar is not closing

    sem_wait(&(sharedData->mutex));


    // if no one is waiting in the buffer it can check in the bar
    if(sharedData->fcfsWaitingBuffer.count == 0){
        int tableIndex = isAnyTableEmpty(sharedData);
        // if there is space in a table and is not occupied
        if(tableIndex != -1){
            sharedData->tables[tableIndex].chairsOccupied++;
            
            //if the table just became full update occupied value
            if(sharedData->tables[tableIndex].chairsOccupied == 4){
                sharedData->tables[tableIndex].isOccupied = true;
            }

            // create a random order (fully random while seed is time)
            srand(time(NULL));
            menuOrder order = randomizeOrder(getpid());

            // put the order in the order buffer
            sharedData->orderBuffer.lastOrders[sharedData->orderBuffer.back] = order;
            sharedData->orderBuffer.back = (sharedData->orderBuffer.back + 1) % 12;
            sharedData->orderBuffer.count++;

        }
    }
    
    // if there is no space in the tables or is someone waiting in the buffer just wait at the buffer
    else{
        int position = sharedData->fcfsWaitingBuffer.back;
        sharedData->fcfsWaitingBuffer.buffer[position] = getpid();
        sharedData->fcfsWaitingBuffer.back = (sharedData->fcfsWaitingBuffer.back + 1) % MAX_VISITORS;
        sharedData->fcfsWaitingBuffer.count++;
        sem_post(&(sharedData->mutex));
        //suspend in this semaphore after unlocking mutex
        sem_wait(&(sharedData->fcfsWaitingBuffer.positionSem[position]));
    }



    sem_post(&(sharedData->mutex));
    
    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);
}