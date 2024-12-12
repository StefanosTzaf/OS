#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <utils.h>

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
    
    sleep(25);

    munmap(sharedData, sharedMemorySize);
    shm_unlink(SHARED_MEMORY_NAME);

    return 0;
}
