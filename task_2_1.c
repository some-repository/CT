#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

ssize_t writeall (int fd, const void *buf, size_t count); //signed size_t, negative values used for errors

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s filename text-to-write\n", argv [0]);
		return 1;
	} 

	int fd = open (argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); // fd - file descriptor, link to file
																 // O_WRONLY - the file will be opened for writing only
                                                                 // O_TRUNC - if the file already exists and is a regular file and the access mode allows writing it will be truncated to length 0
                                                                 // O_CREAT - if pathname does not exist, create it as a regular file
                                                                 // 0644 - user can read and write, others can read
	if (fd < 0)
	{
		perror ("Failed to open file for writing");
		return 2;
	}

	if (writeall (fd, argv [2], strlen (argv [2])) < 0) //strlen(s) calculates the length of the string pointed to by s, excluding the terminating null byte
	{
		perror ("Failed while writing fd");
		close (fd);
		return 3;
	}

	if(close (fd) < 0)
	{
		perror ("Failed while closing fd");
		return 2;
	}
	return 0;
}

ssize_t writeall (int fd, const void *buf, size_t count)
{
	size_t bytes_written = 0; 
	const uint8_t *buf_addr = (const uint8_t*) buf;
	while (bytes_written < count)
	{
		ssize_t res = write (fd, buf_addr + bytes_written, count - bytes_written);
		if (res < 0)
		{
			return res;
		}
		bytes_written += res;
	}
	return (ssize_t) bytes_written; //signed size_t, negative values used for errors
}