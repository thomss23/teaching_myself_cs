//my solution
// #define _GNU_SOURCE
// #include <pthread.h>
// #include <stdint.h>
// #include <stdio.h>
// #include <stdlib.h>

// static void print_stack_usage(unsigned depth) {
//     pthread_attr_t attr;
//     void *stack_base;
//     size_t stack_size;
//     volatile char marker;  // Exists in THIS function's stack frame

//     if (pthread_getattr_np(pthread_self(), &attr) != 0 ||
//         pthread_attr_getstack(&attr, &stack_base, &stack_size) != 0) {
//         perror("getting stack attributes");
//         exit(EXIT_FAILURE);
//     }
//     pthread_attr_destroy(&attr);

//     uintptr_t low = (uintptr_t)stack_base;
//     uintptr_t high = low + stack_size;
//     uintptr_t here = (uintptr_t)&marker;

//     printf("depth %u: approximately %zu / %zu bytes used\n",
//            depth, (size_t)(high - here), stack_size);
// }

// __attribute__((noinline))
// static void stupid(unsigned depth) {
//     print_stack_usage(depth);

//     if (depth < 10)
//         stupid(depth + 1);
// }

// int main(void) {
//     stupid(0);
//     return 0;
// }

//Instructor's solution

#include<stdio.h>
#include<unistd.h>
#include<sys/resource.h>

void f(int depth, long bottom) {
    printf("frame %d %ld (%p)\n", bottom - (long)&depth, &depth);
    return f(depth + 1, bottom);
}

void start() {
    int depth = 0;
    f(depth, (long)&depth);
}

int main() {
    struct rlimit rl;
    getrlmit(RLIMIT_STACK, &rl);
    printf("Current max %llu, hard limit %llu\n", rl.rlim_cur, rl.rlim_max);
    rl.rlim_cur = rl.rlim_max;
    setrlimit(RLIMIT_STACK, &rl);
    printf("Current max %llu, hard limit %llu\n", rl.rlim_cur, rl.rlim_max);
    start();

}