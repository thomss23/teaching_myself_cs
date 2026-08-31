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

int tokenize(char *command, char *argv[], int max_argv) {
    int argc = 0;
    char *token = strtok(command, " \t\n");

    while (token != NULL) {
        if (argc == max_argv - 1) {
            fprintf(stderr, "too many arguments\n");
            return -1;
        }

        argv[argc++] = token;
        token = strtok(NULL, " \t\n");
    }

    argv[argc] = NULL;
    return argc;
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

        char *commands[MAX_ARGV];
        int command_count = 0;

        char *saveptr;
        char *command = strtok_r(input, "|", &saveptr);

        while (command != NULL) {
            if (command_count == MAX_ARGV - 1) {
                fprintf(stderr, "too many commands\n");
                break;
            }

            commands[command_count++] = command;
            command = strtok_r(NULL, "|", &saveptr);
        }

        char *argvs[MAX_ARGV][MAX_ARGV];
        int argcs[MAX_ARGV];

        for (int i = 0; i < command_count; i++) {
            argcs[i] = tokenize(commands[i], argvs[i], MAX_ARGV);

            if (argcs[i] == 0) {
                fprintf(stderr, "empty command in pipeline\n");
                break;
            }
        }   

        if (command_count == 1 && strcmp(argvs[0][0], "help") == 0) {
            help();
        } else if (command_count == 1 && strcmp(argvs[0][0], "quit") == 0) {
            quit();
        }

        if (command_count == 1) {
            int rc = fork();

            if (rc < 0) {
                fprintf(stderr, "fork failed\n");
                exit(1);
            } else if (rc == 0) {
                // child
                signal(SIGINT, SIG_DFL);  // child goes back to normal Ctrl-C behavior
                execvp(argvs[0][0], argvs[0]);
                fprintf(stderr, "exec failed\n");
                exit(1);
            } else {
                // parent
                int wc = wait(NULL);
            }
        } else {
            int input_fd = -1;
            int fd[2];
            int pids[MAX_ARGV];
            for (int i = 0; i < command_count; i++) {

                if (i < command_count - 1) {
                    if (pipe(fd) == -1) {
                        perror("pipe");
                        break;
                    }
                }
                int rc = fork();

                if (rc < 0) {
                    fprintf(stderr, "fork failed\n");
                    exit(1);
                } else if (rc == 0) {
                    // child
                    signal(SIGINT, SIG_DFL);  
                    if (input_fd != -1) {
                        dup2(input_fd, STDIN_FILENO);
                        close(input_fd);
                    } 
                    if (i < command_count - 1) {
                            dup2(fd[1], STDOUT_FILENO);
                            close(fd[0]); 
                            close(fd[1]); 
                    }

                    execvp(argvs[i][0], argvs[i]);

                    fprintf(stderr, "exec failed\n");
                    exit(1);
                } else {
                    // parent
                    pids[i] = rc; // Track child PID to wait for them later
                    if (input_fd != -1) {
                        close(input_fd);
                    }
                    close(fd[1]);  
            
                    input_fd = fd[0]; 
                }
            }
            for (int i = 0; i < command_count; i++) {
                wait(NULL);  // Wait for each child to finish
            }
        }


    }
}



