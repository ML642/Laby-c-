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

void install(char* s1 , char* s2, int size){
            DIR* dir = opendir(".");
            struct dirent* s;
            while((s =readdir(dir))!=NULL){
                if(strcmp(s->d_name,s1)==0){
                    perror("ALREADY EXISTS");
                    return;
                }
            }

            FILE* f1 = fopen(s1,"a");
            
            srand(time(NULL));
            
            for(int i = 0; i < size;i++){
                fprintf(f1, "%c", rand() % 26 + 'A');
            }
            
            fclose(f1);

            FILE* f2 = fopen("requirements","a");

            fprintf(f2,"%s %s\n",s1,s2);
            fclose(f2);  
            closedir(dir);
}

void deleteq(char *s1){
    int find = 0;

   
    if(unlink(s1) && errno == ENOENT){
        ERR("unlink");
    }
    char c1[500];
    char c[500];
    char sd1[500][500];
    char ver[500][500];
    
    int count = 0; 
    FILE* file = fopen("requirements","r+");

    while(fscanf(file,"%s%s",c,c1) == 2){
        if(strcmp(c,s1) != 0){
            strcpy(sd1[count],c);
            strcpy(ver[count],c1);
            count++;
        }
    }
    if(fclose(file)){
        ERR("fclose");
    }
    file = fopen("requirements","w+");

    fseek(file,0,SEEK_SET);

    for(int j = 0 ; j < count ; j++){
        fprintf(file,"%s %s\n",sd1[j],ver[j]);
    }

    if(fclose(file)){
        perror("fclose");
        exit(EXIT_FAILURE);
    }
    
}

int main(int argc,char** argv){
    int i = 0;
    int path_size = 500;
    char curr[500] = ".";
    mode_t perm = 0777;
    char path[500];
    getcwd(path,500);
    
    char* dirrectioins[500];
    int count = 0;

    while((i = getopt(argc,argv,"v:c:i:d:")) != -1){
        if(i == 'c'){
            char* c = optarg;
            mkdir(c,perm);
            chdir(c);

            FILE* x = fopen("requirements","w+");
            fclose(x);
        }
        else if(i == 'v'){
            dirrectioins[count] = optarg;
            count++;
        }
        else if (i == 'i') {
                char path[500];
                getcwd(path, 500);

                char name[500];
                char version[500];

                char *sep = strstr(optarg, "==");

                if (sep == NULL) {
                    fprintf(stderr, "Wrong package format\n");
                    return EXIT_FAILURE;
                }

                int len = sep - optarg;

                strncpy(name, optarg, len);
                name[len] = '\0';

                strcpy(version, sep + 2);

                for (int i = 0; i < count; i++) {
                    chdir(dirrectioins[i]);
                    install(name, version, 500);
                    chdir(path);
                }
            }
        else if(i == 'd'){
            char path[500];
            getcwd(path,500);

            for(int i=0;i<count;i++){
                chdir(dirrectioins[i]);
                deleteq(optarg);
                chdir(path);
            }              
        }
    
    }
    printf("%s\n","KONIEC");
    return EXIT_SUCCESS;
}