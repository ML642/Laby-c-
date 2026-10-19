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

void scanf(){
    DIR* dir = opendir(".");
    if(dir == NULL){
        perror("opendir");
        return ;
    }

    struct dirent* dr;
    
    errno = 0;
    int countDir=0,countFiles=0,countLinks=0;
    int other=0;
    while((dr = readdir(dir))!=NULL){
        struct stat st;
        if(lstat(dr->d_name,&st)){
            perror("lstat");
            return ;
        };

        if(S_ISDIR(st.st_mode)){
            countDir++;
        }
        else if(S_ISLNK(st.st_mode)){
            countLinks++;
        }
        else if(S_ISREG(st.st_mode)){
            countDir++;
        }
        else{
            other++;
        }   
        errno = 0;
    }
    if(dr == NULL){
        if(errno == 0){
            printf("Das ist good\n");
        }
        else{
            perror("readdir");
            return ;
        }
    }
    printf("%s\n%d\n%d\n%d\n%d","We had repositories , links,files, others ",countDir,countFiles,countLinks,other);
    
    if(closedir(dir)){
        perror("closedir");
        return ;
    }
}

int main(int argc,char** argv){
    char buffor[1000];
    if((getcwd(buffor,1000))==NULL){
        perror("getcwd");
        return EXIT_FAILURE;
    }
    
    for(int i=1;i < argc;i++){
        //printf("%s\n",argv[i]);
        if(chdir(argv[i])){
            perror("1 chdir");
            return EXIT_FAILURE;
        }
        scanf();
        
        if(chdir(buffor)){
            perror("2 chdir");
            return EXIT_FAILURE;
        }

    }


    return EXIT_SUCCESS;
}