#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "utils.h"
#include <unistd.h>
#include <getopt.h>


int main(int argc, char* argv[]){

    if(argc != 5){
        fprintf(stderr, "Usage: ./receptionist -d <orderTime> -s sharedMemoryName\n");
        exit(EXIT_FAILURE);
    }
    int option;
    int maxOrderTime;
    char sharedMemoryName[64];

    while((option = getopt(argc, argv, "d:s:")) != -1){
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
        else{
            fprintf(stderr, "Usage: ./receptionist -d <orderTime> -s sharedMemoryName\n");
            exit(EXIT_FAILURE);
        }
    }


    shareDataSegment* sharedData = attachShm(sharedMemoryName);
    size_t sharedMemorySize = sizeof(shareDataSegment);

    while(1){
        sem_wait(&(sharedData->receptionistSem));
        sem_wait(&(sharedData->mutex));
        
        // if there is order to serve
        if (sharedData->orderBuffer.count > 0) {

            int index = sharedData->orderBuffer.front;
            menuOrder currentOrder = sharedData->orderBuffer.lastOrders[index];

            sharedData->sharedStatistics.visitorsServed++;

            if (currentOrder.water) {
                sharedData->sharedStatistics.consumedWater++;
            }
            if(currentOrder.wine){
                sharedData->sharedStatistics.consumedWine++;
            }
            if(currentOrder.cheese){
                sharedData->sharedStatistics.consumedCheese++;
            }
            if(currentOrder.salad){
                sharedData->sharedStatistics.consumedSalads++;
            }

            // awake the first visitor in the queue of ordering in a specific chair FCFS,
            // from now on he can leave the bar after a random time(visitor source code)
            sem_post(&(sharedData->orderBuffer.chairSem[index]));
            
            // updating front (wrap around)
            sharedData->orderBuffer.front = (sharedData->orderBuffer.front + 1) % 12;
        }


        sem_post(&(sharedData->mutex));

        // must awake a visitor to serve him

        // if there is no order to serve AND no one waiting inside the bar to be served AND tables are not occupied
        // AND bar is closing --------> exit (close the bar)
        if(sharedData->closingFlag && 
        sharedData->fcfsWaitingBuffer.count == 0 &&
        sharedData->orderBuffer.count == 0 &&
        sharedData->tables[0].isOccupied == false && sharedData->tables[1].isOccupied == false &&
        sharedData->tables[2].isOccupied == false){
           
            break;  

        }

    }
    
    

    munmap(sharedData, sharedMemorySize);

    exit(EXIT_SUCCESS);
}