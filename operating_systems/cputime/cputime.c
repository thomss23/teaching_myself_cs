#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/resource.h>
#include <sched.h>

#define _GNU_SOURCE  // Required for sched_getcpu()
#define SLEEP_SEC 3
#define NUM_MULS 100000000
#define NUM_MALLOCS 100000
#define MALLOC_SIZE 1000

double timeval_diff(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec)
         + (end.tv_usec - start.tv_usec) / 1000000.0;
}

struct profile_times {
    int pid;
    int core_id;
    struct rusage before;
    struct rusage after;
};

void profile_start(struct profile_times *t) {
    getrusage(RUSAGE_SELF, &t->before);
    t->pid = getpid();
    t-> core_id = sched_getcpu();
}

void profile_log(struct profile_times *t) {
    getrusage(RUSAGE_SELF, &t->after);
    printf("The program is currently running on core: %d\n", t->core_id);
    printf("Pid: %d\n", t->pid);

    printf("  User:   %.6f s\n",
           timeval_diff(t->before.ru_utime,
                        t->after.ru_utime));

    printf("  Kernel: %.6f s\n",
           timeval_diff(t->before.ru_stime,
                        t->after.ru_stime));
}

int main(int argc, char *argv[]) {
    struct profile_times t;

    // Profile floating point multiplications
    volatile float x = 1.0;

    profile_start(&t);

    for (int i = 0; i < NUM_MULS; i++) {
        x *= 1.1f;
    }

    profile_log(&t);


    // Profile mallocs
    profile_start(&t);

    void *p;

    for (int i = 0; i < NUM_MALLOCS; i++) {
        p = malloc(MALLOC_SIZE);
    }

    profile_log(&t);


    // Profile sleeping
    profile_start(&t);

    sleep(SLEEP_SEC);

    profile_log(&t);

    return 0;
}