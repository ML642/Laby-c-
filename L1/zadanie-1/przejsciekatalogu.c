#define _POSIX_C_SOURCE 200809L


#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <time.h>
#include <string.h>


#define MAX_PATH PATH_MAX

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))


char* FILE_OUTPUT = "NO";
int FILE1 = 0;

void usage(){
    printf("p: MUSISZ PODAC SCIEZKE NORMALNA \n");
}

void listing(char* ch ){
    printf("%s\n",ch);
    char* c  = ch;
    DIR* dir = opendir(ch);
    FILE *f;
    if(FILE1){
         f = fopen(FILE_OUTPUT,"a");
        if(f == NULL){
            perror("fopen");
            ERR("fopen");
        }
    }
    if(dir == NULL){
        usage();
        return;
        //ERR("opendir");
        //exit(EXIT_FAILURE);
    }

    if(FILE1){
        fprintf(f,"SCIEZKA:\n");
        fprintf(f,"%s\n",c);
        fprintf(f,"LISTA PLIKOW: \n");
    }
    else{
        printf("SCIEZKA:\n");
        printf("%s\n",c);
        printf("LISTA PLIKOW: \n");
    }

    struct dirent* dr = readdir(dir);

    errno = 0;
    if(dr == NULL){
        if(errno !=0){
            perror("readdir");
            exit(EXIT_FAILURE);
        }
        else{
            printf("KONIEC ODCZYTU\n");
        }
    }
    while(dr != NULL){ 
        if(!FILE1)printf("%s ",dr->d_name);
        else{
            fprintf(f,"%s ",dr->d_name);
        }
        
        struct stat st;

        char fullpath[PATH_MAX];
        sprintf(fullpath, "%s/%s", ch, dr->d_name);
        if(lstat (fullpath,&st)){
            ERR("lstat");
            exit(EXIT_FAILURE);
        };

        if(S_ISDIR(st.st_mode)){
            if (strcmp(dr->d_name, ".") == 0 ||
                strcmp(dr->d_name, "..") == 0) {
            }
            else{
            listing(fullpath);
        }
        }
        
        if(!FILE1)printf("%ld\n",st.st_size);
        else{
            fprintf(f,"%ld\n",st.st_size);;
        }
      
        
        errno = 0;
        dr = readdir(dir);
        if(dr == NULL){
        if(errno !=0){
            perror("readdir");
            exit(EXIT_FAILURE);
        }
        else{
            printf("KONIEC ODCZYTU\n");
        }
    }
    }
    if(closedir(dir)){
        exit(EXIT_FAILURE);
    };
    if(FILE1 && (fclose(f) != 0 )){
        exit(EXIT_FAILURE);
    };
}


int main(int argc,char** argv){
    //listing(".");
    int i;
    // DIR* dir = opendir("sopyyy");
    // if(dir == NULL){
    //     printf("SDFSD\n");
    // }
    int count = 0;
    int counto = 0;

    char *s1[500];
    while( (i = getopt(argc,argv,"o:p:")) != -1){
        if(i == 'p'){
            char* s = optarg;
            s1[count] = optarg;
            count++;
        }
        else if  (i == 'o'){
            FILE_OUTPUT = optarg;
            FILE1 = 1;
            counto++;
            if(counto>1){
                fprintf(stderr,"ONLY ONE O");
                return EXIT_FAILURE;
            }
        }
        else {
            usage();
            return EXIT_FAILURE;
        }
    }
    
    for(int i = 0 ; i < count ;i++){
        listing(s1[i]);
    }

    if(optind != argc){
        printf("UWAGA nie odczytane wszystkie parametry");
    }
    if(count == 0){
        listing(".");
    }
    return EXIT_SUCCESS;
}