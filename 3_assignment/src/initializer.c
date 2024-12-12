#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <utils.h>
#include <sys/wait.h>


#define FORKED_VISITORS 4

int main(int argc, char *argv[]) {
    
    int shmFd;
    size_t sharedMemorySize = sizeof(shareDataSegment);
    shareDataSegment* sharedData;

    //open shared memory
    shmFd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
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
    
    initializeSharedValues(sharedData);
    



    pid_t receptionistPid = fork();
    if (receptionistPid == -1) {
        perror("Error forking receptionist process");
        exit(EXIT_FAILURE);
    }
    else if (receptionistPid == 0) {
        execlp("./receptionist", "./receptionist", NULL);
        exit(EXIT_FAILURE);
    }




    pid_t visitorsPids[FORKED_VISITORS];

    for(int i = 0; i < FORKED_VISITORS; i++) {
        visitorsPids[i] = fork();
        if (visitorsPids[i] == -1) {
            perror("Error forking visitor process");
            exit(EXIT_FAILURE);
        }
        else if (visitorsPids[i] == 0) {
            execlp("./visitor", "./visitor", NULL);
            exit(EXIT_FAILURE);
        }
    }

    int status;
    if(waitpid(receptionistPid, &status, 0) == -1){
        perror("Error waiting for splitter");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < FORKED_VISITORS; i++) {
        if(waitpid(visitorsPids[i], &status, 0) == -1){
            perror("Error waiting for visitor");
            exit(EXIT_FAILURE);
        }
    }
    munmap(sharedData, sharedMemorySize);
    shm_unlink(SHARED_MEMORY_NAME);

    exit(EXIT_SUCCESS);
}
