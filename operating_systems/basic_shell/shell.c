// execute a program by name (search path)
// support a few built-in (e.g quit, help)
// do this in a loop
// no support for strings (tokenize on space and tabs)
// handle EOF and SIGINT correctly (EOF should terminate shell, SIGINT should terminate child)

// 1. Implement REPL, but echoing instead of eval'ing
// 2. tokenize cmd, print argc/argv 
// 3. actually execute (fork/execvp)
// 4. check success criteria incl signal handling 

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


// my solution
#define MAX_CMD 4096
#define MAX_ARGV 256

void help(void) {
    puts("Built-ins: help, quit");
}

void quit(void) {
    exit(0);
}

void main() {
    signal(SIGINT, SIG_IGN);  // shell parent ignores Ctrl-C
    char input[MAX_CMD];
    char* argv[MAX_ARGV];
    int argc;

    while(1) {
        printf("tomshell> ");
        if (fgets(input, sizeof input, stdin) == NULL) {
            break;
        } 

        char* pch;
        argc = 0;
        pch = strtok(input, " \t\n");

        while (pch != NULL) {
            argv[argc] = pch;
            argc++;
            pch = strtok(NULL, " \t\n");
        }    

        if (strcmp(argv[0], "help") == 0) {
            help();
        } else if (strcmp(argv[0], "quit") == 0) {
            quit();
        }
        
        int rc = fork();

        if (rc < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {
            // child
            signal(SIGINT, SIG_DFL);  // child goes back to normal Ctrl-C behavior
            execvp(argv[0], argv);
            fprintf(stderr, "exec failed\n");
            exit(1);
        } else {
            // parent
            int wc = wait(NULL);
        }

    }
}
