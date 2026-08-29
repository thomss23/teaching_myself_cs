#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void signal_handler(int signum) {
    printf("Received signal: %d\n", signum);
    exit(1);
}

int main() {
    signal(SIGINT, signal_handler);
    printf("Running... Press Ctrl+C to send SIGINT or send SIGTERM to terminate.\n");

    while (1) {
        // Simulate some work
    }

    return 0;
}