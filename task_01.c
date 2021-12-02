//includes from man 2 stat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdio.h>

#include <string.h>

#include <time.h>
#include <stdlib.h>
#include <sys/sysmacros.h>
#include <stdint.h>

const int file_type_str_size = 20;
const int time_str_size = 30;

void get_file_type (mode_t st_mode, char *file_type_str);
char *get_utc_time (const time_t *time_cnt, char* time_str);

int main(int argc, char *argv[]) //argv[1] is pointer to the 1st element of the 2nd string which is pathname
{
        struct stat stat_buffer; //file data will be stored here

        if (argc != 2) //there should be only one parameter - pathname
        {
            fprintf(stderr, "Usage: %s <pathname>\n", argv[0]); //print how to use this program
            exit(EXIT_FAILURE);
        }

        if (lstat(argv[1], &stat_buffer) == -1)  //here structure stat_buffer fills by lstat
        {
           perror("lstat"); //error of lstat
           exit(EXIT_FAILURE);
        }

        printf("File:                     %s\n", argv[1]);

        printf("ID of containing device:  [%lx,%lx]\n", (long) major(stat_buffer.st_dev), (long) minor(stat_buffer.st_dev)); 
        //major ID is for the class of the device, minor ID is for the specific instance of the device in that class
        //%lx - long int in hex

        char file_type_str [file_type_str_size]; //here will be stored string describing file type
        get_file_type (stat_buffer.st_mode, &file_type_str[0]);
        printf("File type:                %s", &file_type_str [0]);

        printf("I-node number:            %ld\n", (long) stat_buffer.st_ino);

        printf("Permissions:                     %lo (octal)\n", (unsigned long) ((stat_buffer.st_mode) & 0x1FF)); //0x1FF is bit mask for the last 9 bits

        printf("Link count:               %ld\n", (long) stat_buffer.st_nlink); //number of hard links to the file
        
        printf("Ownership:                UID=%ld   GID=%ld\n", (long) stat_buffer.st_uid, (long) stat_buffer.st_gid); //user ID and group ID of the owner of the file

        printf("Preferred I/O block size: %ld bytes\n", (long) stat_buffer.st_blksize);
        
        printf("File size:                %lld bytes\n", (long long) stat_buffer.st_size);
        
        printf("Blocks allocated:         %lld\n", (long long) stat_buffer.st_blocks); //number of blocks allocated to the file, in 512-byte units.
        //time
        char time_str [time_str_size];
        printf("Last status change:       %s\n", get_utc_time (&stat_buffer.st_ctime, &time_str [0]));
        
        printf("Last file access:         %s\n", get_utc_time (&stat_buffer.st_atime, &time_str [0]));
        
        printf("Last file modification:   %s\n", get_utc_time (&stat_buffer.st_mtime, &time_str [0]));

        exit(EXIT_SUCCESS);
}

void get_file_type (mode_t st_mode, char *file_type_str)
{
	switch (st_mode & S_IFMT) //S_IFMT is a bit mask that we use to get 4 bits of file type (man inode)
    {
        case S_IFBLK:  strcpy (file_type_str, "block device\n");            break;
        case S_IFCHR:  strcpy (file_type_str, "character device\n");        break;
        case S_IFDIR:  strcpy (file_type_str, "directory\n");               break;
        case S_IFIFO:  strcpy (file_type_str, "FIFO/pipe\n");               break;
        case S_IFLNK:  strcpy (file_type_str, "symlink\n");                 break;
        case S_IFREG:  strcpy (file_type_str, "regular file\n");            break;
        case S_IFSOCK: strcpy (file_type_str, "socket\n");                  break;
        default:       strcpy (file_type_str, "unknown\n");                 break;
    }
}

char *get_utc_time (const time_t *time_cnt, char* time_str)
{
	struct tm *time_struct; //time data will be stored here

    time_struct = gmtime (time_cnt); //convert to UTC
    return asctime_r (time_struct, time_str); //returns NULL if the length of the result string more than time_str_size
}