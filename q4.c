#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    printf("Let's explore all the different ways to exec programs (my pid is %d)\n", (int) getpid());
    printf("We'll run 'ls' with each variant to see how they work\n");
    
    // execl() - list arguments individually, need full path
    printf("\n=== Testing execl() ===\n");
    printf("execl() takes arguments as individual parameters\n");
    int child1 = fork();
    if (child1 == 0) {
        execl("/bin/ls", "ls", "-l", NULL);
        // If we get here, exec failed
        perror("execl failed");
        exit(1);
    } else if (child1 > 0) {
        wait(NULL);  // Wait for this child to finish
    }
    
    // execle() - list arguments + environment
    printf("\n=== Testing execle() ===\n");
    printf("execle() is like execl() but lets us set environment variables\n");
    int child2 = fork();
    if (child2 == 0) {
        char *custom_environment[] = {"PATH=/bin:/usr/bin", "HOME=/tmp", NULL};
        execle("/bin/ls", "ls", "-la", NULL, custom_environment);
        perror("execle failed");
        exit(1);
    } else if (child2 > 0) {
        wait(NULL);
    }
    
    // execlp() - list arguments + PATH search
    printf("\n=== Testing execlp() ===\n");
    printf("execlp() searches PATH so we don't need the full /bin/ls path\n");
    int child3 = fork();
    if (child3 == 0) {
        execlp("ls", "ls", "-l", NULL);
        perror("execlp failed");
        exit(1);
    } else if (child3 > 0) {
        wait(NULL);
    }
    
    // execv() - arguments in array, need full path
    printf("\n=== Testing execv() ===\n");
    printf("execv() takes arguments as an array instead of individual parameters\n");
    int child4 = fork();
    if (child4 == 0) {
        char *argument_list[] = {"ls", "-l", NULL};
        execv("/bin/ls", argument_list);
        perror("execv failed");
        exit(1);
    } else if (child4 > 0) {
        wait(NULL);
    }
    
    // execvp() - arguments in array + PATH search
    printf("\n=== Testing execvp() ===\n");
    printf("execvp() combines array arguments with PATH searching\n");
    int child5 = fork();
    if (child5 == 0) {
        char *argument_list[] = {"ls", "-la", NULL};
        execvp("ls", argument_list);
        perror("execvp failed");
        exit(1);
    } else if (child5 > 0) {
        wait(NULL);
    }
    
    // execvpe() - arguments in array + PATH search + environment
    printf("\n=== Testing execvpe() ===\n");
    printf("execvpe() has everything: array args, PATH search, and custom environment\n");
    int child6 = fork();
    if (child6 == 0) {
        char *argument_list[] = {"ls", "-l", NULL};
        char *custom_environment[] = {"PATH=/bin:/usr/bin", "USER=testuser", NULL};
        execvpe("ls", argument_list, custom_environment);
        perror("execvpe failed");
        exit(1);
    } else if (child6 > 0) {
        wait(NULL);
    }
    
    printf("\n=== Summary ===\n");
    printf("Why so many exec variants? Each serves different needs:\n");
    printf("- 'l' vs 'v': List args individually vs use an array\n");
    printf("- 'p': Search PATH automatically vs specify full path\n");
    printf("- 'e': Set custom environment vs inherit parent's environment\n");
    printf("This gives programmers flexibility for different situations!\n");
    
    return 0;
}