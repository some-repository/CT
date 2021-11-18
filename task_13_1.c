#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

//print information about process
void proc_info (const char* procname) 
{
    printf ("[%s]:\n"
           "PID %d,\n"
           "PPID %d,\n"
           "PGID %d,\n"
           "SID %d\n\n", procname, getpid (), getppid (), getpgid (0), getsid (0)); //pgid - process group ID, sid - session ID
}

int main (int argc, char *argv[]) 
{
    pid_t cpid;
    int wstatus;

    cpid = fork(); //create child process
    if (cpid < 0) 
    {
        perror ("fork");
        exit (EXIT_FAILURE);
    }

    if (cpid == 0) //Code executed by child
    {            
        proc_info ("Child");
        if (argc > 1)
        {
            printf ("%s\n", argv [1]);
            exit (atoi (argv[1])); // exit with code from the 1st argument
        }
        exit (42); //if there isn't 1st argument   
    } 
    else //Code executed by parent
    {                      
        if (waitpid (cpid, &wstatus, 0) == -1) //no options
        {
            perror ("waitpid");
            exit (EXIT_FAILURE);
        }

        if (WIFEXITED(wstatus)) 
        {
            printf("the child process has been completed\nstatus = %d\n\n", WEXITSTATUS(wstatus));
            //WEXITSTATUS consists of the least significant 8 bits of the status argument that the child specified in a call to exit()
        } 
        else if (WIFSIGNALED(wstatus)) 
        {
            printf("the child process has been killed by signal %d\n", WTERMSIG(wstatus));
            if (WCOREDUMP(wstatus)) 
            {
                printf("child proc has cause a core dump\n");
            }
        } 

        proc_info ("Parent");       
        exit(EXIT_SUCCESS);
    }
}

/*
WIFEXITED(wstatus)   -- не равно нулю, если дочерний процесс успешно завершился
WIFSIGNALED(wstatus) -- возвращает истинное значение, если дочерний процесс завершился из-за необработанного сигнала
*/