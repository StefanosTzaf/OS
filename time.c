#include <stdio.h>
#include <sys/times.h>
#include <unistd.h>

//calculating cpu and real time
int main (void) {

    double t1, t2, cpu_time;
    //only needed for cpu time
    struct tms tb1, tb2;
    double ticspersec;
    int i, sum = 0;

    ticspersec = (double) sysconf (_SC_CLK_TCK);
    t1 = (double) times (&tb1);

    for (i = 0; i < 100000000; i++){
        sum += i;
    }

    sleep(3); //cpu time wont include sleep time only real

    t2 = (double) times (&tb2);
    cpu_time = (double) (( tb2.tms_utime + tb2.tms_stime) - ( tb1.tms_utime + tb1.tms_stime ));
    printf ("Run time was %lf sec ( REAL time ) although we used the CPU for %lf sec ( CPU time ).\n", (t2 - t1) / ticspersec , cpu_time / ticspersec );

    return 0;
}