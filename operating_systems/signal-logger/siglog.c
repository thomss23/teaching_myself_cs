// #include <signal.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdlib.h>
// #include <string.h>
// #include <stdint.h>

// volatile uint64_t handled = 0;

// void handle(int sig) {
//   handled |= (1 << sig);
//   printf("Caught %d: %s (%d total)\n", sig, strsignal(sig),
//          __builtin_popcount(handled));

//   if (sig == SIGINT) {
//     exit(0);
//   }
// }

// int main(int argc, char* argv[]) {
//     // Register all valid signals
//     for (int i = 0; i < NSIG; i++) {
//         signal(i, handle);
//     } 

//     int rc = fork();

//     if (rc < 0) {
//         fprintf(stderr, "fork failed\n");
//         exit(1);
//     } else if (rc == 0) {
//         // child
//         abort();
//     }

//     // spin
//     for (;;)
//       sleep(1);
// }


#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

volatile uint64_t handled = 0;

void handle(int sig) {
  handled |= (1 << sig);
  printf("Caught %d: %s (%d total)\n", sig, strsignal(sig),
         __builtin_popcount(handled));

  if (sig == SIGINT) {
    exit(0);
  }
}

int main(int argc, char* argv[]) {
    // Register all valid signals
    for (int i = 0; i < NSIG; i++) {
        signal(i, handle);
    } 

    // spin
    for (;;)
      sleep(1);
}
