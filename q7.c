#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("Let's see what happens when we close stdout in a child process\n");
    printf("Parent process before fork (my pid is %d)\n", (int) getpid());
    
    int fork_result = fork();
    if (fork_result < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (fork_result == 0) {
        printf("Child: This will print normally (my pid is %d)\n", (int) getpid());
        
        close(STDOUT_FILENO);
        
        printf("Child: This printf() call happens after closing STDOUT\n");
        printf("Child: You probably won't see this message!\n");
        
        fprintf(stderr, "Child: But stderr still works! (my pid is %d)\n", (int) getpid());
        
        const char *msg = "Child: Trying to write directly to fd 1\n";
        int bytes_written = write(1, msg, 37);
        
        if (bytes_written == -1) {
            fprintf(stderr, "Child: Direct write to fd 1 failed as expected\n");
        } else {
            fprintf(stderr, "Child: Surprisingly, direct write worked and wrote %d bytes\n", bytes_written);
        }
        
        fprintf(stderr, "Child: Finished attempting to write to closed stdout\n");
    } else {
        wait(NULL);
        printf("Parent: Child process completed (my pid is %d)\n", (int) getpid());
    }
    
    printf("\nWhat we learned: Closing stdout makes printf() fail silently\n");
    
    return 0;
}