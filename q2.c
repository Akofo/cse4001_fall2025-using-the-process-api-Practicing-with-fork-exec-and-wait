#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    int fd = open("test_file.txt", O_CREAT|O_WRONLY|O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }
    
    printf("File descriptor: %d (pid:%d)\n", fd, (int) getpid());
    
    int rc = fork();
    if (rc < 0) {
        // fork failed; exit
        fprintf(stderr, "fork failed sad\n");
        exit(1);
    } else if (rc == 0) {
        // child process
        printf("Child: can access fd = %d (pid:%d)\n", fd, (int) getpid());
        const char *child_msg = "Hello from child\n";
        write(fd, child_msg, strlen(child_msg));
        
        // Write multiple times to show concurrent access
        for (int i = 0; i < 3; i++) {
            char buffer[50];
            sprintf(buffer, "Child line %d\n", i);
            write(fd, buffer, strlen(buffer));
            usleep(1000);  // Small delay
        }
        
        close(fd);
    } else {
        // parent process
        printf("Parent: can access fd = %d (pid:%d)\n", fd, (int) getpid());
        const char *parent_msg = "Hello from parent\n";
        write(fd, parent_msg, strlen(parent_msg));
        
        // Write multiple times to show concurrent access
        for (int i = 0; i < 3; i++) {
            char buffer[50];
            sprintf(buffer, "Parent line %d\n", i);
            write(fd, buffer, strlen(buffer));
            usleep(1000);  // Small delay
        }
        
        wait(NULL);  // Wait for child to finish
        close(fd);
        
        // Read and display the file contents
        printf("\nFile contents after both processes wrote:\n");
        system("cat test_file.txt");
    }
    return 0;
}