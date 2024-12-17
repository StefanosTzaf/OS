Command Line Execution :
    Instead of shmid is given the shared memory name because Posix shared memory instructions are used,
    as mentioned in Piazza.

 -- initializer : ./initializer -d <orderTime> -r <restTime> -s <sharedMemoryName> -l <logFileName.txt>
    order time is parameter for receptionist and restTime for visitors. LogFile name is the name that
    we want to give to the file that logging facts will be stored to. If it does not exist it is created.
 
 -- receptionist: ./receptionist -d <orderTime> -s <sharedMemoryName> -l <logFileName.txt\n>
    sharedMemoryName and logFile name should be the same as in initializer! 
    ***For more convinience receptionist is created with fork and exec* inside initializer*** 
 
 -- visitor: ./visitor -d <restTime> -s <sharedMemoryName> -l <logFileName.txt\n>
    SharedMemoryName and logFile name should be the same as in initializer too.
    Some visitors are created in initializer but we are able to create more via command line.
 
 -- monitor: ./monitor -s sharedMemoryName
    it prints the current state of shared memory. Only called by command line.

Shared Memory:
    The main shared memory struct consists of sub-structs to enhance modularity and clean code.

    First sub-struct is about statistics.

    Second sub-struct is about circular waiting buffer that ensure FCFS policy for the first MAX_VISITORS
    (defined in the headerfile and easily modifiable). One semaphore for each position of the buffer ensures
    that only one process would be suspended there. The othe variable have to do with the positions of the buffer
    and wrap-around ability.
    If we had not this buffer fcfs could not be assured while in one semafore are just suspended processes and wake
    up randomly. Now with one semaphore for every buffer position we know who came first.
    
    However this can not defined dynammicaly (shared memory segment has to ve defined before the exwcution). So there is
    an extra semaphore for exceeding visitors intialized to MAX_VISITORS. Only when Fcfs buffer if full visitors will be suspended there
    (without fairness of fcfs in this case).

    Thex next sub-struct is about orders. For orders there is also a circular buffer to ensure that the first customer ordered
    will be served first. It has 12 positions, as many as the chairs (only visitors that have sit can order). These semaphores could
    be located in table struct as well (one semaphore for each chair) but i prefered the first implementation. Also there is a struct
    inside ordersCircularBuffer that indicates what every order consists of (menuOrder)

    Also there are one semaphore for mutual exclusion and one for receptionist (receptioniist will be suspended there whenever
    there is no work to do).

    At the end there is an array of struct table that stores the state of tables (occuoied or not - which chairs etc).

    A bool variable indicates if the bar is closing or not.

Function of the bar:

    After initializer is called, receptionist is made as mentioned. Also some visitors (their number defined) are made too and executed.
    Receptionist at first is suspended in its semaphore while there is not worto do.