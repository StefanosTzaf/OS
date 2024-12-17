#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "utils.h"
#include <unistd.h>
#include <getopt.h>
#include <time.h>

int main(int argc, char* argv[]){

    if(argc != 7){
        fprintf(stderr, "Usage: ./receptionist -d <orderTime> -s sharedMemoryName -l logFileName.txt\n");
        exit(EXIT_FAILURE);
    }
    int option;
    int maxOrderTime;
    char sharedMemoryName[64];
    char logFileName[64];

    while((option = getopt(argc, argv, "d:s:l:")) != -1){
        if(option == 'd'){
            maxOrderTime = atoi(optarg);
            if(maxOrderTime <= 0){
                fprintf(stderr, "Give valid number for orderTime please\n");
                exit(EXIT_FAILURE);
            }
        }
        else if(option == 's'){
            snprintf(sharedMemoryName, sizeof(sharedMemoryName), "/%s", optarg);
        }
        else if(option == 'l'){
            snprintf(logFileName, sizeof(logFileName), "%s", optarg);
        }
        else{
            fprintf(stderr, "Usage: ./receptionist -d <orderTime> -s sharedMemoryName -l logFileName.txt\n");
            exit(EXIT_FAILURE);
        }
    }

    int logFd = open(logFileName,  O_RDWR | O_APPEND , 0666);
    if(logFd == -1){
        perror("log file open failed");
        exit(EXIT_FAILURE);
    }

    shareDataSegment* sharedData = attachShm(sharedMemoryName);
    size_t sharedMemorySize = sizeof(shareDataSegment);
    
    srand(time(NULL));

    while(1){
        sem_wait(&(sharedData->receptionistSem));
        sem_wait(&(sharedData->mutex));
        
        // if there is order to serve
        if (sharedData->orderBuffer.count > 0) {

            int index = sharedData->orderBuffer.front;
            menuOrder currentOrder = sharedData->orderBuffer.lastOrders[index];

            updateStatistics(sharedData, currentOrder);

            int lower = (int)(0.5 * maxOrderTime);
            int randomTime = lower + (rand() % (maxOrderTime - lower + 1));

            // free the mutex before sleeping for a random time
            sem_post(&(sharedData->mutex));
            
            // sleep for a random time preparing a speciffic order
            sleep(randomTime);

            sem_wait(&(sharedData->mutex));
            // awake the first visitor in the queue of ordering in a specific chair FCFS,
            // from now on he can leave the bar after a random time(visitor source code)

            sem_post(&(sharedData->orderBuffer.chairSem[index]));

            
            
            // updating front (wrap around)
            sharedData->orderBuffer.front = (sharedData->orderBuffer.front + 1) % 12;
            sharedData->orderBuffer.count--;

            sem_post(&(sharedData->mutex));
        }

        // if there is no order to serve AND no one waiting inside the bar to be served AND tables are not occupied
        // AND bar is closing --------> exit (close the bar)
        sem_wait(&(sharedData->mutex));

        if(sharedData->closingFlag && 
        sharedData->fcfsWaitingBuffer.count == 0 &&
        sharedData->orderBuffer.count == 0 &&
        sharedData->tables[0].isOccupied == false && sharedData->tables[1].isOccupied == false &&
        sharedData->tables[2].isOccupied == false){
        
            sem_post(&(sharedData->mutex));
            break;  

        }
        sem_post(&(sharedData->mutex));
        
    }
    
    close(logFd);
    munmap(sharedData, sharedMemorySize);

    exit(EXIT_SUCCESS);
}