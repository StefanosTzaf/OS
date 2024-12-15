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

    if(argc != 7){
        fprintf(stderr, "Usage: ./visitor -d <restTime> -s sharedMemoryName -l logFileName.txt\n");
        exit(EXIT_FAILURE);
    }
    int option;
    int maxRestTime;
    char sharedMemoryName[64];
    char logFileName[64];

    while((option = getopt(argc, argv, "d:s:l:")) != -1){
        if(option == 'd'){
            maxRestTime = atoi(optarg);
        }
        else if(option == 's'){
            snprintf(sharedMemoryName, sizeof(sharedMemoryName), "/%s", optarg);
        }
        else if(option == 'l'){
            snprintf(logFileName, sizeof(logFileName), "%s", optarg);
        }
        else{
            fprintf(stderr, "Usage: ./visitor -d <restTime> -s sharedMemoryName\n");
            exit(EXIT_FAILURE);
        }
    }

    int logFd = open(logFileName, O_RDWR | O_APPEND , 0666);
    if(logFd == -1){
        perror("log file open failed");
        exit(EXIT_FAILURE);
    }
    


    shareDataSegment* sharedData = attachShm(sharedMemoryName);
    size_t sharedMemorySize = sizeof(shareDataSegment);

    // fully random seed based to time)
    srand(time(NULL));

    if(sharedData->closingFlag){
        munmap(sharedData, sharedMemorySize);
        close(logFd);
        exit(EXIT_SUCCESS);
    } 


    // if there is no place for visitor to wait inside the bar they should wait outside.
    // So if in this semaphore P() has been applied more than MAX_VISITORS times then the fcfsWaitingBuffer is full 
    sem_wait(&(sharedData->exceedingVisitorsSem));
    


    // if the execution has reached here, there is space in waiting buffer OR the waiting buffer is absolutely free
    // and the bar is NOT closing

    sem_wait(&(sharedData->mutex));

    int chairIndex;
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


            menuOrder order = randomizeOrder(getpid());
            updateStatistics(sharedData, order);

            // put the order in the order buffer
            sharedData->orderBuffer.lastOrders[sharedData->orderBuffer.back] = order;
            chairIndex = sharedData->orderBuffer.back;
            sharedData->orderBuffer.back = (sharedData->orderBuffer.back + 1) % 12;
            sharedData->orderBuffer.count++;

            sem_wait(&(sharedData->orderBuffer.chairSem[chairIndex]));

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

        //when it is awake, it can order
        menuOrder order = randomizeOrder(getpid());
        sharedData->orderBuffer.lastOrders[sharedData->orderBuffer.back] = order;
        chairIndex = sharedData->orderBuffer.back;
        sharedData->orderBuffer.back = (sharedData->orderBuffer.back + 1) % 12;
        sharedData->orderBuffer.count++;

    }

    sem_post(&(sharedData->mutex));
    // it should be suspended in the semaphore of his chair
    sem_wait(&(sharedData->orderBuffer.chairSem[chairIndex]));



    sem_wait(&(sharedData->mutex));
    // now the visitor should have been served (because only receptionist can awake him from the semaphore with orders!!)
    //so it has to wait a random time -- eat and discuss --and then leave the bar

    int lower = (int)(0.7 * maxRestTime);
 
    // random integer in the interval [lower *0.7, maxRestTime]
    int randomTime = lower + (rand() % (maxRestTime - lower + 1));

    sem_post(&(sharedData->mutex));

    sleep(randomTime);



    //TODO to inform others to sit if he is the last one in the table


    close(logFd);
    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);

}