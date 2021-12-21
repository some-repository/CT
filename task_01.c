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

const char* get_file_type (mode_t st_mode);
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
        
        printf("File type:                %s\n", get_file_type (stat_buffer.st_mode));

        printf("I-node number:            %ld\n", (uintmax_t) stat_buffer.st_ino);

        printf("Permissions:                     %lo (octal)\n", (unsigned long) ((stat_buffer.st_mode) & ALL_PERMS)); //CHANGE TO ALL_PERMS 12 bit mask

        printf("Link count:               %ld\n", (long) stat_buffer.st_nlink); //number of hard links to the file
        
        printf("Ownership:                UID=%ld   GID=%ld\n", (long) stat_buffer.st_uid, (long) stat_buffer.st_gid); //user ID and group ID of the owner of the file

        printf("Preferred I/O block size: %ld bytes\n", (long) stat_buffer.st_blksize);
        
        printf("File size:                %lld bytes\n", (long long) stat_buffer.st_size);
        
        printf("Blocks allocated:         %lld\n", (long long) stat_buffer.st_blocks); //number of blocks allocated to the file, in 512-byte units.
        																			   //ADD BLOCK SIZE
        //time
        char time_str [time_str_size];
        printf("Last status change:       %s\n", get_utc_time (&stat_buffer.st_ctime, time_str));
        
        printf("Last file access:         %s\n", get_utc_time (&stat_buffer.st_atime, time_str));
        
        printf("Last file modification:   %s\n", get_utc_time (&stat_buffer.st_mtime, time_str));

        exit(EXIT_SUCCESS);
}

const char* get_file_type (mode_t st_mode)
{
	switch (st_mode & S_IFMT) //S_IFMT is a bit mask that we use to get 4 bits of file type (man inode)
    {
        case S_IFBLK:  return "block device";
        case S_IFCHR:  return "character device";
        case S_IFDIR:  return "directory";
        case S_IFIFO:  return "FIFO/pipe";
        case S_IFLNK:  return "symlink";
        case S_IFREG:  return "regular file";
        case S_IFSOCK: return "socket";
        default:       return "unknown";
    }
}

char *get_utc_time (const time_t *time_cnt, char* time_str)
{
	struct tm *time_struct; //time data will be stored here

    time_struct = gmtime (time_cnt); //convert to UTC CHANGE TO gmtime_r()
    return asctime_r (time_struct, time_str); //returns NULL if the length of the result string more than time_str_size CHANGE TO STRFTIME()
}