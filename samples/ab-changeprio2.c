#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>


static const char c = 'A';

#define SCHED_DUMMY 6

// set priority on i-th process
static void set_priority(unsigned i, pid_t child_pid) {
    int prio = 0;

    // change these values as you wish
    // prio ranges from -20 (PRIO_MIN) to 20 (PRIO_MAX)
    //
    // careful: a lower number corresponds to a more favorable scheduling,
    // i.e. a process with -20 (PRIO_MIN) will have highest priority, whereas 
    // a process with 20 (PRIO_MAX) will only be scheduled when there is nothing
    // else to be scheduled.

    switch(i) {
    case 0: prio = 16; break;
    case 1: prio = 17; break;
    case 2: prio = 18; break;
    case 3: prio = 19; break;
    case 4: prio = 20; break;
    }

    setpriority(PRIO_PROCESS, child_pid, prio);

    printf("setting priority of %c(%d) to %d\n", c + i, (int)child_pid, prio);
}

static void loop_on(char c) {
    int prio, error;
    unsigned i;

    prio = getpriority(PRIO_PROCESS, 0);
    printf("prio = %d\n", prio);
    if(prio == 12) {
        error = setpriority(PRIO_PROCESS, 0, 15);
        prio = getpriority(PRIO_PROCESS, 0);
        printf("error = %s, new prio = %d\n", strerror(error), prio);
    }

    for(i=0; i<500000000; ++i) {
        if(i % 10000000 == 0) {
			if(i % 100000000 == 99999999) {
				setpriority(PRIO_PROCESS, 0, 25);
			}
            putchar(c);
            fflush(stdout);
        }
    }
}

int main(int argc, char* argv[]) {
    pid_t pid;
    unsigned i;

    // create 5 child processes
    for(i=0; i<5; ++i) {
        pid = fork();
        if(pid < 0) { // error
            return 1;
        } else if(pid == 0) { // child
            loop_on(c + i);
            exit(0); // child exits
        }
        // parent continues ..
        set_priority(i, pid);
    }

    return 0;
}
