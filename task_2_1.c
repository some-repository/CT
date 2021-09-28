#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

ssize_t writeall (int fd, const void *buf, size_t count)
{
	ssize_t bytes_written = 0; 
	const uint8_t *buf_addr = buf;
	while (bytes_written < count)
	{
		ssize_t res = write (fd, buf_addr + bytes_written, count - bytes_written);
		if (res < 0)
		{
			return res;
		}
		bytes_written += res;
	}
	return (ssize_t) bytes_written; //signed size_t, negative values are for errors
}

int main (int argc, char *argv[])
{
	if (argc != 3)
	{
		fprintf (stderr, "Usage: %s filename text-to-write\n", argv [0]);
		return 1;
	} 
	int fd = open (argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0644); //fd - file descriptor, link to file
	if (fd < 0)
	{
		perror ("Failed to open file for writing");
		return 2;
	}

	if (writeall (fd, argv [2], strlen (argv [2])) < 0)
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