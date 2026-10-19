#define _POSIX_C_SOURCE 200809L

//#include "fseek.c"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>

#define MAX_PATH PATH_MAX

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))


void make_file(
    char* name,
    int size,
    mode_t permissions,
    int t
){
    FILE* s1;
    int i = 0 ;
    umask(~permissions & 0777);

    s1 = fopen(name,"w+");

    if(s1 == NULL){
        ERR("fopen");
    }
    for(int i=0; i < (size*t)/100 ;i++){
        if(fseek(s1,rand()%size,SEEK_SET)){
            ERR("fseek");
        }
        fprintf(s1,"%c",'A' + (i%('Z' - 'A' + 1)));
    }
    if(fclose(s1)){
        ERR("flose");
    }
}


int main(int argc,char** argv){
    char* name = NULL;
    mode_t permission = 0 ;
    long size = 0;

    int opt;
    while((opt = getopt(argc,argv,"n:p:s:")) != -1){
        if(opt == 'n'){
            name = optarg;
        }
        if(opt == 'p'){
            permission = strtol(optarg,NULL,8);
        }
        if(opt == 's'){
            size = strtol(optarg,NULL,10);
        }
    }
    if (name == NULL || size <= 0) return EXIT_FAILURE;
    
    srand(time(NULL));

    make_file(name,size,permission,10);
}