#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

void get(char *s){
    FILE* filed = fopen(s,"r");

    if(filed == NULL){
        perror("error");
        exit(EXIT_FAILURE);
    }

    char buffer[100];
    int counter = 1;
    while(fgets(buffer,100,filed) != NULL ){
        printf("%d %s",counter,buffer);
        counter++;
    }

    if(fclose(filed)){
        perror("fclose");
        exit(EXIT_FAILURE);
    };
}

void fseek_(char* c){
    FILE* fl = fopen(c,"r");
    if(fl== NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fseek(fl,0,SEEK_END);
    long  fl1 = ftell(fl);
    if(fl1 == -1L){
        perror("fseek");
        exit(EXIT_FAILURE);
    }

    printf("%ld ", fl1);

    if(fseek(fl,0,SEEK_SET)!=0){
        perror("fseek");
        exit(EXIT_FAILURE);
    };


    if(fseek(fl,5,SEEK_CUR)!=0){
        perror("fseek");
        exit(EXIT_FAILURE);
    };

    char cha;
    fscanf(fl,"%c",&cha);

    printf("%c",cha);

    if(fclose(fl)){
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

void create(char* s){
    FILE* fl = fopen(s,"w+");

    int x[5] ={1,2,3,4,5};

    int read[3];
    fwrite(x,sizeof(int),5,fl);

    fseek(fl,0,SEEK_SET);

    int read1 = fread(read,sizeof(int),10,fl);
    if(feof(fl)){
        printf("GOOD");
    }

    if(ferror(fl)){
        printf("BAAD");
    }



    printf("%d was read\n",read1);

    if(fclose(fl)){
        perror("error");
    };
}

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

void get(char *s){
    FILE* filed = fopen(s,"r");

    if(filed == NULL){
        perror("error");
        exit(EXIT_FAILURE);
    }

    char buffer[100];
    int counter = 1;
    while(fgets(buffer,100,filed) != NULL ){
        printf("%d %s",counter,buffer);
        counter++;
    }

    if(fclose(filed)){
        perror("fclose");
        exit(EXIT_FAILURE);
    };
}

void fseek_(char* c){
    FILE* fl = fopen(c,"r");
    if(fl== NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fseek(fl,0,SEEK_END);
    long  fl1 = ftell(fl);
    if(fl1 == -1L){
        perror("fseek");
        exit(EXIT_FAILURE);
    }

    printf("%ld ", fl1);

    if(fseek(fl,0,SEEK_SET)!=0){
        perror("fseek");
        exit(EXIT_FAILURE);
    };


    if(fseek(fl,5,SEEK_CUR)!=0){
        perror("fseek");
        exit(EXIT_FAILURE);
    };

    char cha;
    fscanf(fl,"%c",&cha);

    printf("%c",cha);

    if(fclose(fl)){
        perror("fclose");
        exit(EXIT_FAILURE);
    }
}

void create(char* s){
    FILE* fl = fopen(s,"w+");

    int x[5] ={1,2,3,4,5};

    int read[3];
    fwrite(x,sizeof(int),5,fl);

    fseek(fl,0,SEEK_SET);

    int read1 = fread(read,sizeof(int),10,fl);
    if(feof(fl)){
        printf("GOOD");
    }

    if(ferror(fl)){
        printf("BAAD");
    }



    printf("%d was read\n",read1);

    if(fclose(fl)){
        perror("error");
    };
}

void rawscan(){
    FILE* fl = fopen("NEW_FILE","w+");

    if(fl == NULL){
        exit(EXIT_FAILURE);
    }
    char text[20] = "321213123"; 

    fwrite(text,sizeof(char),5,fl);

    fseek( fl,5,SEEK_SET);

    unlink("NEW_FILE");

    char some[5];

    fread(some,sizeof(char),5,fl);

    for(size_t i=0;i < strlen(some);i++){
        printf("%c ",some[i]);
    }
    
    if(fclose(fl)){
        perror("fclose");
        exit(EXIT_FAILURE);
    };

}   