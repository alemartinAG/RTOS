#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

static int i = 0;

//Thread function to be invoked when the periodic timer expires
void sigHandler (union sigval val) {
    printf("Handler entered with value :%d for %d times\n", val.sival_int, ++i);
}

int main() {

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    struct sched_param parm;
    parm.sched_priority = 255;
    pthread_attr_setschedparam(&attr, &parm);

    struct sigevent sig;
    sig.sigev_notify = SIGEV_THREAD;
    sig.sigev_notify_function = sigHandler;
    sig.sigev_value.sival_int = 20;
    sig.sigev_notify_attributes = &attr;

    //create a new timer.
    timer_t timerid;

    if(timer_create(CLOCK_REALTIME, &sig, &timerid) == 0) {

        struct itimerspec in, out;
        in.it_value.tv_sec = 1;
        in.it_value.tv_nsec = 0;
        in.it_interval.tv_sec = 0;
        in.it_interval.tv_nsec = 100000000;
        //issue the periodic timer request here.
        if(timer_settime(timerid, 0, &in, &out) == 0) {
            sleep(2);
        }
        else {
            printf("timer_settime() failed with %d\n", errno);
        }

        timer_delete(timerid);
    }
    else {
        printf("timer_create() failed with %d\n", errno);
    }

    return 0;
}