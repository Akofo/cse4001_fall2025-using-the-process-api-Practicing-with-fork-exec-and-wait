#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("Let's see how waitpid() gives us more control than wait()\n");
    printf("Parent process starting (my pid is %d)\n", (int) getpid());
    
    // Create multiple children to show waitpid()'s advantages
    int child1_pid = fork();
    if (child1_pid == 0) {
        printf("Child 1 starting (my pid is %d) - I'll take 3 seconds\n", (int) getpid());
        sleep(3);
        printf("Child 1 finishing (my pid is %d)\n", (int) getpid());
        exit(1);  // Exit with status 1
    }
    
    int child2_pid = fork();
    if (child2_pid == 0) {
        printf("Child 2 starting (my pid is %d) - I'll take 1 second\n", (int) getpid());
        sleep(1);
        printf("Child 2 finishing (my pid is %d)\n", (int) getpid());
        exit(2);  // Exit with status 2
    }
    
    // Parent process - demonstrate waitpid() advantages
    printf("Parent created children with PIDs: %d and %d\n", child1_pid, child2_pid);
    printf("With regular wait(), we'd have to wait for whichever finishes first\n");
    printf("But with waitpid(), we can wait for specific children!\n");
    
    // Wait specifically for child2 first (even though child1 was created first)
    int status;
    printf("\nParent deciding to wait specifically for child2 (pid %d) first\n", child2_pid);
    int wait_result = waitpid(child2_pid, &status, 0);
    printf("waitpid() returned %d for child2, exit status: %d\n", 
           wait_result, WEXITSTATUS(status));
    
    // Now wait for child1
    printf("Now waiting specifically for child1 (pid %d)\n", child1_pid);
    wait_result = waitpid(child1_pid, &status, 0);
    printf("waitpid() returned %d for child1, exit status: %d\n", 
           wait_result, WEXITSTATUS(status));
    
    // Demonstrate non-blocking waitpid with WNOHANG
    printf("\n=== Non-blocking waitpid() with WNOHANG ===\n");
    
    int child3_pid = fork();
    if (child3_pid == 0) {
        printf("Child 3 starting (my pid is %d) - I'll take 2 seconds\n", (int) getpid());
        sleep(2);
        printf("Child 3 finishing (my pid is %d)\n", (int) getpid());
        exit(3);
    }
    
    // Try non-blocking wait immediately
    printf("Trying non-blocking waitpid() on child3 right away...\n");
    int nonblocking_result = waitpid(child3_pid, &status, WNOHANG);
    if (nonblocking_result == 0) {
        printf("Child3 is still running (waitpid returned 0 with WNOHANG)\n");
    } else if (nonblocking_result == child3_pid) {
        printf("Child3 finished already! Exit status: %d\n", WEXITSTATUS(status));
    }
    
    // Do some work while child is running
    printf("Parent doing other important work while child3 runs...\n");
    for (int i = 0; i < 3; i++) {
        printf("Parent working... %d\n", i);
        sleep(1);
    }
    
    // Try non-blocking wait again
    printf("Checking on child3 again...\n");
    nonblocking_result = waitpid(child3_pid, &status, WNOHANG);
    if (nonblocking_result == 0) {
        printf("Child3 still running somehow - let's wait for it normally\n");
        waitpid(child3_pid, &status, 0);
        printf("Child3 finally finished, exit status: %d\n", WEXITSTATUS(status));
    } else {
        printf("Child3 finished while we were working! Exit status: %d\n", WEXITSTATUS(status));
    }
    
    printf("\n=== Why waitpid() is useful ===\n");
    printf("1. Wait for specific children by PID instead of any child\n");
    printf("2. Non-blocking option (WNOHANG) lets us check without waiting\n");
    printf("3. More control over which child to wait for in what order\n");
    printf("4. Useful when you have multiple children doing different tasks\n");
    
    return 0;
}