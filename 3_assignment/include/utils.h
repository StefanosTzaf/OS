#include "shm_structs.h"

void initializeSharedValues(shareDataSegment *sharedData);

shareDataSegment* attachShm(char* sharedMemoryName);

int isAnyTableEmpty(shareDataSegment* sharedData);

menuOrder randomizeOrder(pid_t visitorID);

void updateStatistics(shareDataSegment* sharedData, menuOrder currentOrder);