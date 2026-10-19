#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200809L


#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
//#include <sys/uio.h>
#include <unistd.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

// join 2 path. returned pointer is for newly allocated memory and must be freed
char* join_paths(const char* path1, const char* path2)
{
    char* res;
    const int l1 = strlen(path1);
    if (path1[l1 - 1] == '/')
    {
        res = malloc(strlen(path1) + strlen(path2) + 1);
        if (!res)
            ERR("malloc");
        strcpy(res, path1);
    }
    else
    {
        res = malloc(strlen(path1) + strlen(path2) + 2);  // additional space for "/"
        if (!res)
            ERR("malloc");
        strcpy(res, path1);
        res[l1] = '/';
        res[l1 + 1] = 0;
    }
    return strcat(res, path2);
}

void usage(int argc, char** argv)
{
    (void)argc;
    fprintf(stderr, "USAGE: %s path\n", argv[0]);
    exit(EXIT_FAILURE);
}

void seek_(char* dirname,char* title2, char *genre2){
    FILE* fl = fopen(dirname,"r");
    
    if(fl == NULL){
        ERR("fopen");
    }

    char author[500];
    int author1 = 0;
    char title[500];
    int title1 = 0;
    char genre[500];
    int genre1 = 0;

    char c[500];
    // fgets copies with \n
    while(fgets(c,500,fl)!=NULL){
        char *r = NULL;
        char s[500];
        char val[500];

        //now r points to the :

        r = strchr(c,':');
        if(r == NULL){
            perror("THe line does not contain :");
            continue;
        }
        //coping before the array
        strncpy(s,c,r-c);
        s[r-c] = '\0';
        //copying after the symbol
        strcpy(val,r);

        if(strcmp(s,"author")==0){
            strcpy(author,val+1);
            author1 = 1;
        }
        else if(strcmp(s,"title")==0){
            strcpy(title,val+1);
            title1 = 1;
        }
         else if(strcmp(s,"genre")==0){
            strcpy(genre,val+1);
            genre1 = 1;
         }

        //printf(" KEY: %s |  VALUE: %s\n",s,val+1);
    }
    if(author1){
       // strcpy(author2, author);

        printf("Author : %s",author);
    }
    else printf("Author : missing!\n");

    if(title1){
         strcpy(title2, title);
        printf("Title : %s",title);
    }
    else printf("Title : missing!\n");

    if(genre1){
        strcpy(genre2,genre);
        printf("Genre : %s",genre);}
    else printf("Genre : missing! \n");

    if(fclose(fl)){
        ERR("fclose");
    }
}

void  recursivesearch(char * dir,char* linpath,char* pathtofile , char* linpathtitle,char* linpathgenre){
    DIR* directory = opendir(dir);

    if(directory == NULL){
        ERR("opendir");
    }
    struct dirent* rd;

    errno = 0;
    while((rd = readdir(directory)) != NULL){
        struct stat st;
        
        char buffor[500];
        char linpath_c[500];



        char pathtofile1[500];
        // copying the files 
        sprintf(buffor,"%s/%s",dir,rd->d_name);
        sprintf(linpath_c,"%s/%s",linpath,rd->d_name);
        sprintf(pathtofile1,"%s/%s",pathtofile,rd->d_name);

        if(lstat(buffor,&st)){
            ERR("lstat");
        }

        if(S_ISDIR(st.st_mode) && (strcmp(rd->d_name,".") != 0) && (strcmp(rd->d_name,"..")!=0)){
            recursivesearch(buffor,linpath,pathtofile1,linpathtitle,linpathgenre);
        }
        if(S_ISREG(st.st_mode)){
            char title[500] = "";
            char genre[500] = "";

            // char lintitle[500];
            // char lingenre[500];
            
            if(symlink(pathtofile1,linpath_c)){
                ERR("symlink");
            }
            seek_(buffor,title,genre);
            
            if(strlen(title) > 64)
                title[64] = '\0';
            if(strlen(genre)>64){
                genre[64] = '\0';
            }   
            char *lintitle = join_paths(linpathtitle, title);

            char *lingenre = join_paths(linpathgenre,genre);
            
            
            if(title[0] != '\0'){
                if(symlink(pathtofile1,lintitle)){
                ERR("symlink");
                }
            }
            if(genre[0] != '\0'){
                if(symlink(pathtofile1,lingenre)){
                ERR("symlink");
                }
            }
            free(lintitle);
            free(lingenre);
           
        }   
    }
    if(rd == NULL){
        if(errno != 0 ){
            ERR("rd");
        }
        else{
            //koniec pliku
        }
    }
    if(closedir(directory)){
        ERR("closedir");
    }
}

void Recursie_search(){
    if(mkdir("index",0777)){
        if(errno == EEXIST){
            ERR("CATALOG IS ALREADY HERE");
        }
        else{
            ERR("mkdir");
        }
    }
    

    char buffor[500];

    if(getcwd(buffor,500)==NULL){
        ERR("getcwd");
    };

    if(chdir("index")){
        ERR("chdir");
    };

    if(mkdir("by-visible-title",0777)){
        if(errno == EEXIST){
            ERR("CATALOG IS ALREADY HERE");
        }
        else{
            ERR("mkdir");
        }
    }
    
    mkdir("by-title", 0777);
    mkdir("by-genre", 0777);

    if(chdir(buffor)){
        ERR("chdir");
    }
    
    recursivesearch( "./library",
    "./index/by-visible-title",
    "../../library",
    "./index/by-title",
    "./index/by-genre");
}

int main(int argc, char** argv) {
    char buffor[500];

    if(getcwd(buffor,500)== NULL){
        ERR("getcwd");
    }

    if(chdir("library")){
        ERR("chdir");
    }

   // seek_(argv[1]);
    
    if(chdir(buffor)){
        ERR("chdir");
    }
    Recursie_search();
    return EXIT_SUCCESS;

    //usage(argc, argv);
}
