#include <stdio.h>
#include <semaphore.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

int main(int argc, char* argv[]){
    char* name = argv[1];


    shm_unlink(name);
}