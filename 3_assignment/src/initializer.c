#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>

#define SHARED_MEMORY_NAME "/bar_in_nemea"
#define SHARED_MEMORY_SIZE 1024
#define SEM_NAME "/nemea_table_sem"

// Δομή για shared memory
typedef struct {

    int tables[3][4];         //condition of each chair
    int visitors;             //total visitors
    float averageWaitingTime; //average waiting time
    float averageStayTime;    //average consumption
    int consumedWine;         //total wine consumed
    int consumedWater;        //total water consumed
    int consumedCheese;       //total cheese consumed
    int consumedSalads;       //total salads consumed

} shared_data;

int main(int argc, char *argv[]) {

    int shmFd;
    shared_data* sharedData;
    sem_t *sem;

    //open shared memory
    shmFd = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0666);
    if (shmFd == -1) {
        perror("shared memory open failed");
        exit(EXIT_FAILURE);
    }

    //define the size
    if (ftruncate(shmFd, SHARED_MEMORY_SIZE) == -1) {
        perror("ftruncate failed");
        exit(EXIT_FAILURE);
    }

    //Map shared memory in current address space
    sharedData = mmap(0, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    if (sharedData == MAP_FAILED) {
        perror("mmap failed");
        exit(EXIT_FAILURE);
    }

    // Αρχικοποίηση shared memory
    memset(sharedData, 0, sizeof(shared_data));

    // // Δημιουργία semaphore για συγχρονισμό τραπεζιών
    // sem = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0666, 1);
    // if (sem == SEM_FAILED) {
    //     perror("sem_open failed");
    //     exit(EXIT_FAILURE);
    // }
    sharedData->visitors++;

    close(shmFd);
    munmap(sharedData, SHARED_MEMORY_SIZE);

    return 0;
}
