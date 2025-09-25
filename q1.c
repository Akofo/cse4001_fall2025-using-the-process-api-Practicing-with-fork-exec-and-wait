#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int x = 100;  // Initialize variable x to 100
    
    printf("Before fork: x = %d (pid:%d)\n", x, (int) getpid());
    
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed :/ tragic\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        printf("Child: x = %d (pid:%d)\n", x, (int) getpid());
        x = 200;  // Change x in child
        printf("Child after change: x = %d (pid:%d)\n", x, (int) getpid());
    } else {
        // parent process
        printf("Parent: x = %d (pid:%d)\n", x, (int) getpid());
        x = 300;  // Change x in parent
        printf("Parent after change: x = %d (pid:%d)\n", x, (int) getpid());
        
        wait(NULL);  // Wait for child to finish
        printf("Parent final: x = %d (pid:%d)\n", x, (int) getpid());
    }
    return 0;
}