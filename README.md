# Assignment: Practicing the Process API
Practicing with fork, exec, wait. 

### Overview

In this assignment, you will practice using the Process API to create processes and run programs under Linux. The goal is to gain hands-on experience with system calls related to process management. Specifically, you will practice using the unix process API functions 'fork()', 'exec()', 'wait()', and 'exit()'. 

⚠️ Note: This is not an OS/161 assignment. You will complete it directly on Linux. 

Use the Linux in your CSE4001 container. If you are using macOS, you may use the Terminal (you may need to install development tools with C/C++ compilers). 

**Reference Reading**: Arpaci-Dusseau, *Operating Systems: Three Easy Pieces*, Chapter 5 (Process API Basics)
 👉 [Chapter 5 PDF](http://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf)

---

### **Steps to Complete the Assignment**

1. **Accept the GitHub Classroom Invitation**
    [GitHub Link](https://classroom.github.com/a/FZh4BrQG)
2. **Set up your Repository**
   - Clone the assignment repository.
3. **Study the Reference Materials**
   - Read **Chapter 5**.
   - Download and explore the sample programs from the textbook repository:
      [OSTEP CPU API Code](https://github.com/remzi-arpacidusseau/ostep-code/tree/master/cpu-api).
4. **Write Your Programs**
   - Adapt the provided example code to answer the assignment questions.
   - Each program should be clear, well-commented, and compile/run correctly.
   - Add your solution source code to the repository.

5. **Prepare Your Report**
   - Answer the questions in the README.md file. You must edit the README.md file and not create another file with the answers. 
   - For each question:
     - Include your **code**.
     - Provide your **answer/explanation**.
6. **Submit Your Work via GitHub**
   - Push both your **program code** to your assignment repository.
   - This push will serve as your submission.
   - Make sure all files, answers, and screenshots are uploaded and rendered properly.








---
### Questions
1. Write a program that calls `fork()`. Before calling `fork()`, have the main process access a variable (e.g., x) and set its value to something (e.g., 100). What value is the variable in the child process? What happens to the variable when both the child and parent change the value of x?

![Question 1 Screenshot](Screenshot%20(1).png)
```cpp
The child process initially receives the same value of the variable (100) because fork() creates an exact duplicate of the parent's address space. When both processes modify their respective copies of x, the changes remain isolated within each process's memory space. The child's modification to 200 occurs independently of the parent's modification to 300. This demonstrates that fork() establishes complete memory separation between processes. The operating system maintains distinct virtual address spaces for each process, ensuring that memory modifications in one process cannot affect another process's data structures.
Code:
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
```


2. Write a program that opens a file (with the `open()` system call) and then calls `fork()` to create a new process. Can both the child and parent access the file descriptor returned by `open()`? What happens when they are writing to the file concurrently, i.e., at the same time?
![Question 2 Screenshot](Screenshot%20(2).png)
```cpp
Both the child and parent processes can access the same file descriptor returned by open(). File descriptors are inherited during fork() and reference the same underlying file table entry in the kernel. Both processes share the file position pointer and can perform concurrent write operations. When writing simultaneously, the output becomes interleaved based on the operating system's process scheduling decisions. The order of writes depends on which process receives CPU time at any given moment. This shared access mechanism enables inter-process communication through files and pipes while requiring careful coordination to prevent race conditions.
Code:
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
```

3. Write another program using `fork()`.The child process should print “hello”; the parent process should print “goodbye”. You should try to ensure that the child process always prints first; can you do this without calling `wait()` in the parent?

![Question 3 Screenshot](Screenshot%20(3).png)
```cpp
Ensuring the child process prints before the parent without using wait() is unreliable due to non-deterministic process scheduling. The operating system scheduler determines which process executes first based on system load, process priorities, and scheduling algorithms. Using sleep() in the parent increases the probability that the child executes first but provides no guarantee. The scheduler may preempt the child process or delay its execution regardless of the parent's sleep duration. Reliable process ordering requires explicit synchronization mechanisms rather than timing-based approaches.
Code:
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
```


4. Write a program that calls `fork()` and then calls some form of `exec()` to run the program `/bin/ls`. See if you can try all of the variants of `exec()`, including (on Linux) `execl()`, `execle()`, `execlp()`, `execv()`, `execvp()`, and `execvpe()`. Why do you think there are so many variants of the same basic call?

![Question 4 Screenshot](Screenshot%20(4).png)
```cpp
The six exec() variants provide different interfaces for program execution while maintaining the same core functionality of replacing the current process image. Functions with 'l' accept individual argument parameters, while 'v' variants take argument arrays. Variants containing 'p' search the PATH environment variable for the executable. Functions ending in 'e' allow specification of custom environment variables for the new process. This design provides programming flexibility for different argument handling scenarios, path resolution requirements, and environment configuration needs while maintaining consistent process replacement semantics.
Code:
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
```

5. Now write a program that uses `wait()` to wait for the child process to finish in the parent. What does `wait()` return? What happens if you use `wait()` in the child?

![Question 5 Screenshot](Screenshot%20(5).png)
```cpp
wait() returns the process ID of the terminated child process and stores the child's exit status in the provided parameter. If no child processes exist, wait() returns -1 immediately and sets errno to ECHILD. When a child process calls wait(), it returns -1 with errno set to ECHILD because child processes have no children to wait for. This mechanism prevents zombie processes by allowing parents to collect child termination information and maintains the hierarchical structure of the process tree.
Code:
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
```

6. Write a slight modification of the previous program, this time using `waitpid()` instead of `wait()`. When would `waitpid()` be useful?

![Question 6 Screenshot](Screenshot%20(6).png)
```cpp
 waitpid() provides targeted process synchronization by allowing parents to wait for specific child processes identified by process ID rather than any available child. The WNOHANG option enables non-blocking operation, returning immediately with status information if the specified child has terminated or zero if it remains active. This functionality proves essential when coordinating multiple child processes with distinct roles, implementing selective process waiting, or building responsive applications that require periodic status checking without blocking execution.
Code:
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
```

7. Write a program that creates a child process, and then in the child closes standard output (`STDOUT FILENO`). What happens if the child calls `printf()` to print some output after closing the descriptor?

![Question 7 Screenshot](Screenshot%20(7).png)
```cpp
Closing STDOUT_FILENO severs the process's connection to standard output, causing subsequent printf() calls to fail silently without generating output or error messages. The process loses access to file descriptor 1 while maintaining access to other file descriptors such as stderr. This behavior demonstrates the independence of file descriptor channels and the operating system's approach to resource management. Each file descriptor represents a distinct communication pathway that can be individually controlled without affecting other I/O streams.
Code:
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
```

