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

void usage(){
    printf("./main -n name -p 0777 -s 999");
    ERR("wrong usage");
}

void record(int size,char* name, mode_t params){
    
    umask(~params & 0777);

    FILE* fl = fopen(name,"w+");

    if(chmod(fl,0777)){
        ERR(fl);
    }

    if(fl == NULL){
        ERR("fl");
    }

    for(int i = 0 ; i < size/10;i++){
        int ra = rand() % size;
        if(fseek(fl,ra,SEEK_SET) !=0){
            ERR("fseek");
        };

        if(fputc('A'+i%26,fl) == EOF){
            ERR("fputc");
        };
    }
    if(fclose(fl)){
        ERR("fl");
    }
}

int main(int argc,char** argv){

    int i,size=-1;
    char* name = NULL;
    mode_t perm = -1;
    while((i=getopt(argc,argv,"n:p:s:")) != -1){
        if(i == 'n'){
            name = optarg;
        }
        else if(i == 'p'){
            perm = strtol(optarg,NULL,8); 
        }
        else if(i == 's'){
            size = strtol(optarg,NULL,10);
        }
        else {
            //usage();
        }
    }

    if(size<=0 || size>50000){
        usage();
    }
    if( perm > 0777 || perm < 0000){
        usage();
    }
    

    if(optind != argc){
        printf("NOT ALL WERE MENTIONED");
    }
    srand(time(NULL));
    record(size,name,perm);

    return EXIT_SUCCESS;
}