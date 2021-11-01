#include <stdio.h>

int main(int argc, char *argv[])
       {
               fprintf(stdout, "in_0: %s, in_1: %c \n", argv[0], *argv[1]);
               return 1;
       }
