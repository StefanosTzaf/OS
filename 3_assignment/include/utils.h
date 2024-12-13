#include "shm_structs.h"

void initializeSharedValues(shareDataSegment *sharedData);

shareDataSegment* attachShm(char* sharedMemoryName);