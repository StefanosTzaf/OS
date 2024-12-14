#include <unistd.h>
#include <semaphore.h>
#include <stdbool.h>

#define MAX_VISITORS 50
//struct for statistics of the bar
struct statistics{

    float averageWaitingTime; // average waiting time
    float totalWaitingTime;   // total waiting time

    float averageStayTime;    // average consumption
    float totalStayTime;      // total consumption

    int consumedWine;         // total wine consumed
    int consumedWater;        // total water consumed
    int consumedCheese;       // total cheese consumed
    int consumedSalads;       // total salads consumed

    int visitorsServed;             //total visitors

};

// struct for circular buffer managment implemenitig FCFS policy inside the bar
struct waitingCircularBuffer{

    pid_t buffer[MAX_VISITORS];
    int front;
    int back;
    int count;
    sem_t positionSem[MAX_VISITORS];

};

struct table{

    bool isOccupied;     // if table is occupied and receptionist cannot assign it to a visitor
    pid_t chairs[4];     // who has sit in each chair
    int chairsOccupied;  // how many chairs are occupied

};

// what every visitor choosed from menu 
typedef struct{

    pid_t visitor;
    bool water;
    bool wine;
    bool cheese;
    bool salad;

} menuOrder;

// struct for circular buffer managment for orders (FCFS again)
struct orderCircularBuffer{

    menuOrder lastOrders[12];
    int front;
    int back;
    int count;
    // semaphore for each CHAIR (each visitor that it will sit in a chair will lock there till it will order)
    // only one process it would be suspended in each semaphore for chair
    sem_t chairSem[12];

};


typedef struct {

    struct statistics sharedStatistics;
    
    struct waitingCircularBuffer fcfsWaitingBuffer;

    sem_t exceedingVisitorsSem; // semaphore for exceeding visitors initialized to max_visitors
    
    struct orderCircularBuffer orderBuffer;
    
    struct table tables[3];

    sem_t mutex; // mutex for shared memory access

    sem_t receptionistSem; // semaphore for receptionist, when it has no work to do it will sleep in this semaphore
   
    // if a process that is called to close the bar has set this flag to true,
    // visitors will be served but no more will be accepted

    // !! VISITORS SHOULD CHECK WHEN COMING THIS BOOL TO LEAVE INSTANTLY IF TRUE
    bool closingFlag; 

} shareDataSegment;
