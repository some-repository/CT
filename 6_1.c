#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>

int main (void)
{
	DIR *dir_fd = opendir ("/home"); //current directory
	if (dir_fd == NULL)
	{
		perror ("opendir");
		return 1;
	}
	struct dirent *entry;
	while ((entry == readdir (dir_fd)) != NULL)
	{
		puts (entry -> d_name);
	}
	closedir (dir_fd);
	return 0;
}