#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>

int main() {
    if (sem_unlink("/receptionist") == -1) {
        perror("sem_unlink failed");
        return 1;
    }
    printf("Semaphore 'receptionist' unlinked successfully.\n");
    return 0;
}