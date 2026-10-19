#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>


void mk(char *path){
    mode_t x = 0755;
    mkdir(path,x);

    chdir(path);

    FILE* fl =  fopen("file.txt","w+");

    mode_t right = 0444;
    chmod("file.txt",right);

    access("file.txt",F_OK);
    
    access("file.txt",R_OK);
 
    access("file.txt",W_OK);

    mode_t right2 = 0666;

    chmod("file.txt",right2);

    access("file.txt",W_OK);

    fclose(fl);
};