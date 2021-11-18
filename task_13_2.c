#include <sys/wait.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void proc_info (const char* procname);

int main (int argc, char *argv[]) 
{
    pid_t cpid, parent_pid;
    int wstatus;
    siginfo_t data;

    parent_pid = getpid ();

    cpid = fork(); //create child process
    if (cpid < 0) 
    {
        perror ("fork");
        exit (EXIT_FAILURE);
    }

    if (cpid == 0) //Code executed by child
    {            
        proc_info ("Child");
        kill (parent_pid, SIGTERM); //kill parent
        while (getppid () == parent_pid); //wait until child will not be reparented
        proc_info ("Child");
        exit (EXIT_SUCCESS);
    } 
    else //Code executed by parent
    {                      
        while (1);
    }
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

/*
WIFEXITED(wstatus)   -- не равно нулю, если дочерний процесс успешно завершился
WIFSIGNALED(wstatus) -- возвращает истинное значение, если дочерний процесс завершился из-за необработанного сигнала
*/