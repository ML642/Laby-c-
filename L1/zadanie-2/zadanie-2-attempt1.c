#define _POSIX_C_SOURCE 200809L

//#include "fseek.c"
#include <string.h>
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
    printf("./program -p filedir");
}


void scan_dir(char* s,char* s2,int depth,FILE* file){
    DIR* dir = opendir(s);
    if(dir == NULL){
        ERR("dir");
    }

    struct dirent* dr = NULL;
    errno = 0;
    
    if(file == NULL)printf("%s%s\n","path: ",s);
    else{
        fprintf(file,"%s%s\n","path: ",s);
    }
    while((dr=readdir(dir)) !=NULL){
        if(errno != 0){
            ERR("opendir");
        }
        struct stat st;

        char buff[1000];
        if(strcmp(dr->d_name,".") == 0 || strcmp(dr->d_name,"..") == 0)continue;
        


        if(sprintf(buff,"%s/%s",s,dr->d_name) == -1){
             ERR("sprintf");
        };
       
        
        
        //printf("%s",buff);
        
        if(lstat(buff,&st)){
            ERR("lstat");
        }
        if(S_ISDIR(st.st_mode)){
                        if(depth>1){
                        scan_dir(buff,s2,depth-1,file);
                        // printf("SCANNING");
                        }
                    }

        char* a = strchr(dr->d_name,'.');
        if(s2 && a==NULL)continue;

        if(s2){
            if( strcmp(a+1,s2) == 0){
                //printf("%s %ld\n",buff,st.st_size);
                if(file == NULL)printf("%s %ld\n",buff,st.st_size);
                else{
                    fprintf(file,"%s %ld\n",buff,st.st_size);
                }
        }
        }
        else{
             if(file == NULL)printf("%s %ld\n",buff,st.st_size);
                else{
                    fprintf(file,"%s %ld\n",buff,st.st_size);
                }
        }
        errno = 0;
    }
    if(closedir(dir)){
        ERR("closedir");
    }
}

int main(int argc,char** argv){

    int i = 0;

    char* buffor[500];
    int countbuffor = 0;

    
    char* cond = NULL;
    int depth = 5;
    char* file = NULL;

    while((i = getopt(argc,argv,"p:e:d:o:")) != -1){
        if(i == 'p'){
            buffor[countbuffor] = optarg;
            countbuffor++;
        }
        else if(i == 'e'){
            cond = optarg;
        }
        else if(i == 'd'){
            depth = strtol(optarg,NULL,10);
        }
        else if(i == 'o'){
            file = getenv("L1_OUTPUTFILE");
        }
        else{
            usage();
            return EXIT_FAILURE;
        }
    }
    FILE* File = fopen(file,"w+");

    for(int i = 0;i<countbuffor;i++){
        scan_dir(buffor[i],cond,depth,File);
    }

    if(File && fclose(File)!=0){
        perror("fclose");
        return EXIT_FAILURE;
    }   
    return EXIT_SUCCESS;
}