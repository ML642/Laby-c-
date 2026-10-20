#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void func(char* f , int rec, FILE* file){
   // printf("pregladam katalog %s, rekurencyjnie, z wyjsciem do pliku %s ",f , file);
    
    int CountLinks = 0;
    int CountReg = 0;
    char* max;
    int maxlength = -999;
   // printf("dafds");
    DIR* dirr = opendir(f);
    if(dirr == NULL){
        ERR("opendir");
    }
  //  printf("dafds");
    struct dirent* dr;
    while((dr = readdir(dirr))!=NULL){
        //printf("cool\n");
        struct stat st;
        
        char buffor[560];
        
        sprintf(buffor,"%s/%s",f,dr->d_name);

        if(lstat(buffor,&st)){
            ERR("lstat");
        }

        if(S_ISLNK(st.st_mode)){
            CountLinks++;
        }
        if(S_ISDIR(st.st_mode)){
            CountReg++;

            if(rec){
                if( (strcmp(dr->d_name,".")!=0 ) && (strcmp(dr->d_name,"..") != 0) )func(buffor,rec,file);
            }
        }
        
        //printf("%ld\n",strlen(dr->d_name));

        if(maxlength < (int)strlen(dr->d_name)){
            max = dr->d_name;
            maxlength = (int)strlen(dr->d_name);
        }


        //printf("%d\n",maxlength);
        errno = 0;
    }
    if(file == NULL){
    
    printf("mamy %d dirrekor \n", CountReg);
    printf("mamy %d linkow \n", CountLinks);
    printf("najdluzsza imie %s ma %d dlugosc\n",max,maxlength);
    }
    else{
        char buff[500];
        if(getcwd(buff,500)==NULL){
            ERR("getcwd");
        }
        fprintf(file,"%s,\n",buff);
        fprintf(file,"mamy %d dirrekor \n", CountReg);
        fprintf(file,"mamy %d linkow \n", CountLinks);
        fprintf(file,"najdluzsza imie %s ma %d dlugosc\n",max,maxlength);
    }
    if(dr == NULL){
        if(errno!=0){
            ERR("readdir");
        }
    }
    if(closedir(dirr)){
        ERR("closedir");
    }

}

int main(int argc,char** argv){
    
    int i;

    char* buffor[500];
    int count = 0;
    int rec = 0;
    
    char* file=NULL;

    while((i = getopt(argc,argv,"d:o:r")) != -1){
        if(i == 'd'){
            buffor[count] = optarg;
            count++;
        }
        else if(i == 'r'){
            rec = 1;
        }
        else if(i == 'o'){
            for(int i = 0 ; i < strlen(optarg); i++){
                if( (optarg[i] < 'a' || optarg[i] > 'z') && optarg[i] != '.' ){
                    printf("WRONG formating");
                    return EXIT_FAILURE;
                }
            }

            file = optarg;
        }
        else{
            printf("za duzo dannych\n");
        }
    }

    if(optind != argc){
        printf("za duzo dannych\n");
    }

    FILE* fl = NULL;
    if(file != NULL){
        fl = fopen(file,"w+");
            if(fl == NULL){
                    ERR("fl");
            }    
    }
   

    for(int i = 0; i < count;i++){
        //printf("%s\n",buffor[i]);
        func(buffor[i],rec,fl);
    }
    
    if(file && fclose(fl)){
        ERR("fclose");
    }

    return EXIT_SUCCESS;
}