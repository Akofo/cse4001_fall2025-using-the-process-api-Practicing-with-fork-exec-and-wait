#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("Let's see who can print first - parent or child!\n");
    
    int fork_result = fork();
    if (fork_result < 0) {
        // Something went wrong with creating the child process
        fprintf(stderr, "fork failed - couldn't create child process\n");
        exit(1);
    } else if (fork_result == 0) {
        // I'm the child process - let me say hello right away
        printf("hello\n");
    } else {
        // I'm the parent process - let me wait a bit to give child a head start
        // Note: This isn't guaranteed to work every time due to scheduling
        sleep(1);  
        printf("goodbye\n");
    }
    return 0;
}