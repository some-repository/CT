#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

void proc_info (const char* procname);

int main (int argc, char* argv[]) 
{
    pid_t cpid = fork (); //create child process
    if (cpid < 0) 
    {
        perror ("fork");
        exit (EXIT_FAILURE);
    }

    if (cpid == 0) //Code executed by child process
    {
        int file_fd = open ("14_1_child.txt", O_WRONLY | O_APPEND | O_CREAT, 0644); // the information about the child process will be written to 14_1_child.txt
                                                                                    // O_WRONLY - the file will be opened for writing only
                                                                                    // O_APPEND - before each write, the file offset is positioned at the end of the file
                                                                                    // O_CREAT - if pathname does not exist, create it as a regular file
                                                                                    // 0644 - user can read and write, others can read
        if (file_fd < 0) 
        {
            perror ("file_fd");
            exit (EXIT_FAILURE);
        }

        if (dup2 (file_fd, STDOUT_FILENO) == -1) //if successful, STDOUT_FILENO becomes fd of 14_1_child.txt
        {
            perror ("dup2");
            exit (EXIT_FAILURE);
        }

        close (file_fd); //close old fd of 14_1_child.txt
        proc_info ("child"); //process information of child will be written to 14_1_child.txt
    }
    else //Code executed by parent process
    {
        proc_info ("parent"); //process information of parent will be written to terminal
    }

    exit (EXIT_SUCCESS);
}

//print information about process
void proc_info (const char* procname) 
{
    printf ("[%s]:\n"
           "PID %d,\n"
           "PPID %d,\n"
           "PGID %d,\n"
           "SID %d\n\n", procname, getpid (), getppid (), getpgid (0), getsid (0)); //pgid - process group ID, sid - session ID
}