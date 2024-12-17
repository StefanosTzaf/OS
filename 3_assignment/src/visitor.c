#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include "utils.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/time.h>
#include <string.h>


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
            fprintf(stderr, "Usage: ./visitor -d <restTime> -s sharedMemoryName -l logFileName.txt\n");
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

    // fully random seed based to time
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long seed = tv.tv_sec * 1000000 + tv.tv_usec;
    srand(seed);

    //logging data
    char buffer[256];
    sprintf(buffer,"\n[INFO] Visitor with ID: %d has just arrived\n", getpid());
    write(logFd, buffer, strlen(buffer));

    if(sharedData->closingFlag){
        sprintf(buffer,"\n[LEAVING] Visitor with ID: %d has just left because bar is closing\n", getpid());
        write(logFd, buffer, strlen(buffer));
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
    int tableIndex = isAnyTableEmpty(sharedData);

    // if no one is waiting in the buffer he can check in the bar, if there is space in a table and is not occupied sit there
    if(sharedData->fcfsWaitingBuffer.count == 0 && tableIndex != -1){

        // visitor did not wait in the buffer so exceeding buffer semaphore should be incremented
        sem_post(&(sharedData->exceedingVisitorsSem));


        sitInTheFirstEmptyChair(sharedData, getpid(), tableIndex);

        menuOrder order = randomizeOrder(getpid(), logFd);

        // put the order in the order buffer
        sharedData->orderBuffer.lastOrders[sharedData->orderBuffer.back] = order;

        // chair to sleep 
        chairIndex = sharedData->orderBuffer.back;
        sharedData->orderBuffer.back = (sharedData->orderBuffer.back + 1) % 12;
        sharedData->orderBuffer.count++;

    }
    
    // if there is no space in the tables or is someone waiting in the buffer just wait at the buffer's end (FCFS)
    else{
        int position = sharedData->fcfsWaitingBuffer.back;
        sharedData->fcfsWaitingBuffer.buffer[position] = getpid();
        sharedData->fcfsWaitingBuffer.back = (sharedData->fcfsWaitingBuffer.back + 1) % MAX_VISITORS;
        sharedData->fcfsWaitingBuffer.count++;

        sem_post(&(sharedData->mutex));
        //suspend in this semaphore after unlocking mutex
        sem_wait(&(sharedData->fcfsWaitingBuffer.positionSem[position]));


        // here the visitor has been awaken by a last leaving visitor so this visitor has incremeted the excededing buffer semaphore not here
        sem_wait(&(sharedData->mutex));

        tableIndex = isAnyTableEmpty(sharedData);
        sitInTheFirstEmptyChair(sharedData, getpid(), tableIndex);

        //when he awakes, he can order
        menuOrder order = randomizeOrder(getpid(), logFd);
        sharedData->orderBuffer.lastOrders[sharedData->orderBuffer.back] = order;
        chairIndex = sharedData->orderBuffer.back;
        sharedData->orderBuffer.back = (sharedData->orderBuffer.back + 1) % 12;
        sharedData->orderBuffer.count++;

    }

    // inform receptionist that there is an order to serve
    sem_post(&(sharedData->receptionistSem));

    sem_post(&(sharedData->mutex));

    // it should be suspended in the semaphore of his chair (after mutex unlock)
    sem_wait(&(sharedData->orderBuffer.chairSem[chairIndex]));



    sem_wait(&(sharedData->mutex));
    // now the visitor should have been served (because only receptionist can awake him from the semaphore with orders!!)
    //so it has to wait a random time -- eat and discuss --and then leave the bar

    int lower = (int)(0.7 * maxRestTime);
 
    sprintf(buffer, "\n[SERVED] Visitor with ID: %d has been served and is now eating. \n", getpid());
    write(logFd, buffer, strlen(buffer));

    // random integer in the interval [lower *0.7, maxRestTime]
    int randomTime = lower + (rand() % (maxRestTime - lower + 1));

    sem_post(&(sharedData->mutex));

    sleep(randomTime);
    
    sem_wait(&(sharedData->mutex));
    // visitor has finished eating and is leaving the bar
    sprintf(buffer, "\n[LEAVE] Visitor with ID: %d has just left the bar\n", getpid());
    write(logFd, buffer, strlen(buffer));
    
    // defensive case
    if(tableIndex != -1){
        sharedData->tables[tableIndex].chairsOccupied--;
        //inform others that the chair is empty
        int chair = findChairInTable(sharedData, getpid(), tableIndex);
        sharedData->tables[tableIndex].chairs[chair] = -1;


        if(sharedData->tables[tableIndex].chairsOccupied == 0){
            sharedData->tables[tableIndex].isOccupied = false;
            lastVisitorInformingOthers(sharedData, tableIndex);
        }
    }

    sem_post(&(sharedData->mutex));


    close(logFd);
    munmap(sharedData, sharedMemorySize);
    exit(EXIT_SUCCESS);

}