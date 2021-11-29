#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>

const unsigned int max_length = 1024 * 1024; //1 MB

ssize_t writeall (int fd, const void *buf, size_t count); //signed size_t, negative values used for errors
int remove_file (const char* filename);
int copy_regular_file (unsigned int source_fd, unsigned int destination_fd, const char* destination_file, struct stat* stat_buffer, const unsigned int max_length);

int main (int argc, char* argv []) 
{
	//source file path ptr is in argv [1], destination file path ptr is in argv [2]
    if (argc != 3) 
    {
        fprintf (stderr, "Usage: %s source_file_path destination_file_path\n", argv [0]);
        exit (EXIT_FAILURE);
    }
    
    struct stat stat_buffer;
    if (lstat (argv [1], &stat_buffer) == -1)  //here structure stat_buffer fills by lstat
    {
        perror ("lstat"); //error of lstat
        exit (EXIT_FAILURE);
    }

    if ((stat_buffer.st_mode & S_IFMT) != S_IFREG) //check if source file is regular
    {
        fprintf (stderr, "File %s is not a regular file\n", argv [1]);
        exit (EXIT_FAILURE);
    }

    int source_fd = open (argv [1], O_RDONLY);//O_RDONLY - the file will be opened for reading only
    if (source_fd < 0) 
    {
        perror ("Failed to open source file for reading");
        exit (EXIT_FAILURE);
    }

    int destination_fd = open (argv [2], O_WRONLY | O_CREAT | O_TRUNC, 0644);//O_WRONLY - the file will be opened for writing only
                                                                 			 //O_TRUNC - if the file already exists and is a regular file and the access mode allows writing it will be truncated to length 0
                                                                 			 //O_CREAT - if pathname does not exist, create it as a regular file
                                                                 			 //0644 - user can read and write, others can read
    if (destination_fd < 0) 
    {
        perror ("Failed to open destination file for writing");
        exit (EXIT_FAILURE);
    }

    copy_regular_file (source_fd, destination_fd, argv[2], &stat_buffer, max_length);
    
	if (close (source_fd) < 0) 
	{
        perror ("Failed to close source fd");
        exit (EXIT_FAILURE);
    }

    if (close (destination_fd) < 0) 
    {
        perror ("Failed to close destination fd");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}

int copy_regular_file (unsigned int source_fd, unsigned int destination_fd, const char* destination_file, struct stat* stat_buffer, const unsigned int max_length) 
{
    char* buf = (char*) calloc (max_length, sizeof (char)); //allocating initialized by 0 buffer for max_length chars
    if (buf == NULL)
    {
    	fprintf (stderr, "buf pointer is NULL\n");
    	exit (EXIT_FAILURE);
    }

    size_t remain_quantity = (size_t) stat_buffer -> st_size;

    while (remain_quantity > 0) 
    {
        ssize_t received_quantity = read (source_fd, buf, max_length);
        if (received_quantity < 0) 
        {
            perror ("Failed to read from the source file");
            remove_file (destination_file);
            exit (EXIT_FAILURE);
        }

        ssize_t written_quantity = writeall (destination_fd, buf, received_quantity);

        if (written_quantity < 0) 
        {
            perror("Failed to write to the destination file");
            remove_file (destination_file);
            exit (EXIT_FAILURE);
        }

        if (written_quantity != received_quantity) 
        {
            perror ("The number of written bytes is not equal to the number of read bytes");
            remove_file (destination_file);
            exit (EXIT_FAILURE);
        }

        remain_quantity -= received_quantity;
    }

    free (buf);
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

int remove_file (const char* filename) 
{
    if (unlink (filename) == -1) 
    {
        perror("Failed to remove file");
        return -1;
    }
    return 0;
}
