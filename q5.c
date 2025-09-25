#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("Let's explore what wait() does and what happens when a child calls it\n");
    printf("Parent process starting (my pid is %d)\n", (int) getpid());
    
    int fork_result = fork();
    if (fork_result < 0) {
        // fork failed
        fprintf(stderr, "fork failed - couldn't create child process\n");
        exit(1);
    } else if (fork_result == 0) {
        // I'm the child process
        printf("Child process running (my pid is %d)\n", (int) getpid());
        printf("Child is doing some work...\n");
        sleep(2);  // Simulate some work taking time
        printf("Child process finishing (my pid is %d)\n", (int) getpid());
        exit(42);  // Exit with a specific status code so parent can see it
    } else {
        // I'm the parent process
        printf("Parent waiting for child to finish (my pid is %d)\n", (int) getpid());
        
        int child_status;
        int wait_return_value = wait(&child_status);
        
        printf("Parent: wait() returned %d (this should be the child's PID)\n", wait_return_value);
        printf("Parent: child's exit status was %d\n", WEXITSTATUS(child_status));
        printf("Parent process finishing (my pid is %d)\n", (int) getpid());
    }
    
    // Now let's see what happens when a child tries to call wait()
    printf("\n=== What happens when a child calls wait()? ===\n");
    
    int second_fork = fork();
    if (second_fork < 0) {
        fprintf(stderr, "second fork failed\n");
        exit(1);
    } else if (second_fork == 0) {
        // I'm a child process with no children of my own
        printf("Child attempting to call wait() (my pid is %d)\n", (int) getpid());
        int wait_result = wait(NULL);
        printf("Child: wait() returned %d\n", wait_result);
        if (wait_result == -1) {
            perror("Child's wait() call failed because");
            printf("This makes sense - I have no children to wait for!\n");
        }
    } else {
        // I'm the parent of the second child
        wait(NULL);  // Wait for this child to finish
        printf("Parent: Second experiment completed\n");
    }
    
    printf("\n=== Summary ===\n");
    printf("wait() returns the PID of the child that finished\n");
    printf("wait() also gives us the child's exit status\n");
    printf("If a process with no children calls wait(), it fails with ECHILD error\n");
    
    return 0;
}