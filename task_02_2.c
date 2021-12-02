#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main (int argc, char *argv [])
{
	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s filename text_to_write\n", argv [0]);
		exit (EXIT_FAILURE);
	} 

	int fd = open (argv [1], O_WRONLY | O_CREAT | O_TRUNC, 0644);// fd - file descriptor, link to file
																 // O_WRONLY - the file will be opened for writing only
                                                                 // O_TRUNC - if the file already exists and is a regular file and the access mode allows writing it will be truncated to length 0
                                                                 // O_CREAT - if pathname does not exist, create it as a regular file
                                                                 // 0644 - user can read and write, others can read
	if (fd < 0)
	{
		perror ("Failed to open file for writing");
		exit (EXIT_FAILURE);
	}

	if (dprintf (fd, "%s", argv [2]) < 0)  //we don't need to check if all characters from the passed string were written as it was with write()
	{
        perror ("Failed to write to file");
        close (fd);
        exit (EXIT_FAILURE);
    }

    if (close (fd) < 0) 
    {
        perror ("Failed to close file");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}