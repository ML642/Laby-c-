#define _POSIX_C_SOURCE 200809L

#include "fseek.c"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#define MAX_PATH PATH_MAX

#define ERR(source) \
    (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void scan_dir(void)
{
    DIR *dir;
    struct dirent *entry;
    struct stat st;

    if ((dir = opendir(".")) == NULL)
        ERR("opendir");

    while (1)
    {
        errno = 0;
        entry = readdir(dir);

        if (entry == NULL)
            break;

        if (lstat(entry->d_name, &st))
            ERR("lstat");

        if (S_ISREG(st.st_mode))
        {
            printf("FILE: %s\n", entry->d_name);
        }
        else if (S_ISDIR(st.st_mode))
        {
            printf("DIR: %s\n", entry->d_name);
        }
        else if (S_ISLNK(st.st_mode))
        {
            printf("LINK: %s\n", entry->d_name);
        }
        else
        {
            printf("OTHER: %s\n", entry->d_name);
        }
    }

    if (errno != 0)
        ERR("readdir");

    if (closedir(dir))
        ERR("closedir");
}

void tellsize(){
    FILE* fl = fopen("numbers.txt","r");

    if(!fl) {
        ERR("fopen");
    }

    fseek(fl,0,SEEK_END);

    if(fseek(fl,0,SEEK_END) !=0 ) exit(EXIT_FAILURE);


    long ftel = ftell(fl);

    if(ftel == -1L){
        perror("ftell");
        exit(EXIT_FAILURE);
    }

    if(fseek(fl,0,SEEK_SET)!=0){
        exit(EXIT_FAILURE);
    };

    
    printf("%ld\n",ftel);
    
    if(fclose(fl) != 0) exit(EXIT_FAILURE);
}


int main()
{
    FILE* fl = fopen("numbers.txt","r+");
    get("numbers.txt");




    int x = 101;
    int y = 202;
    int z = 303; 
    tellsize();
    fprintf(fl,"%d %d %d",x,y,z);

    if(fclose(fl)){
        perror("fclose");
        return EXIT_FAILURE;
    };
    FILE* fl1 = fopen("numbers.txt","r");
    
    int q,w,e;

    fscanf(fl1,"%d %d %d",&q,&w,&e);

    printf("%d%d%d\n",q,w,e);
    create("LALALAL");

    if(fclose(fl1)){
        perror("fclose");
        return EXIT_FAILURE;
    };

    // char path[MAX_PATH];

    // if (getcwd(path, MAX_PATH) == NULL)
    //     ERR("getcwd");

    // for (int i = 1; i < argc; i++)
    // {
    //     if (chdir(argv[i]))
    //         ERR("chdir");

    //     printf("%s:\n", argv[i]);

    //     scan_dir();

    //     if (chdir(path))
    //         ERR("chdir");
    // }

    return EXIT_SUCCESS;
}