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

ssize_t bulk_read(int fd, char *buf, size_t count)
{
    ssize_t c;
    ssize_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(read(fd, buf, count));
        if (c < 0)
            return c;
        if (c == 0)
            return len;  // EOF
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}

ssize_t bulk_write(int fd, char *buf, size_t count)
{
    ssize_t c;
    ssize_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(write(fd, buf, count));
        if (c < 0)
            return c;
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}




void show_stage2(const char *const path, const struct stat *const stat_buf) {
    if(S_ISREG(stat_buf->st_mode)){
        printf("Rozmiar pliku w bajtach : %ld\n",stat_buf->st_size);
        return;
    }

    if(S_ISDIR(stat_buf->st_mode)){
        DIR* dir = opendir(path);
        
        if(dir == NULL){
            ERR("opendir");
        }

        struct dirent* dr;
        errno = 0; 
        while((dr = readdir(dir))!= NULL){
            printf("%s\n",dr->d_name);
        }
        if(dr == NULL){
            if(errno!=0){
                ERR("readdir");
            }
            else{
                printf("Wszystkie pliki przeczytane \n");
            }
        }
        
        if(closedir(dir)){
        ERR("closedir");
         }
        
        return;
    }
    printf("nieznany typ linku");
}

void write_stage3(const char *const path, const struct stat *const stat_buf){
    FILE* f = fopen(path,"a");

    if(f == NULL){
        ERR("fopen");
    }
    char buff[500]="";
    while(fgets(buff,sizeof(buff),stdin) != NULL ){
            if(strcmp(buff,"\n")==0){
                if(fclose(f)){
                ERR("fclose");
                }
                return;
            }
            fprintf(f,"%s",buff); 
            if(fflush(f)){
                ERR("fflush");
            }           
    }
    if(fclose(f)){
                ERR("fclose");
                return;
    }
}

int func(const char *fpath, const struct stat *sb,
                   int typeflag){
                    printf("%s\n",fpath);
                    if(S_ISDIR(sb->st_mode)){
                        printf("%s\n","this is a directory");
                        return 0;
                    }
                    if(S_ISREG(sb->st_mode)){
                                                printf("%s\n","this is a file");
                                                return 0;
                    }
                     printf("%s\n","we dont know this file");
                    return 0;

}

void walk_stage4(const char *const path, const struct stat *const stat_buf){
    ftw(path,func,100);
}

int interface_stage1() {
    printf("%s\n%s\n%s\n%s\n","1. show","2. write","3. walk","4. exit");

    int x;
    scanf("%d",&x);

    if(x > 4 || x < 1){
        printf("%s","nie poprawny argument\n");
        return 1;
    }
    
    if(x == 4){
        return 0;
    }
    printf("prosze podac sciezke do pliku\n");
    char buffor[500];
    scanf("%s",buffor);

    struct stat xt;
    if(stat(buffor,&xt)){
        printf("NIE ISTNIEJE SCIEZKA");
        return 1;
    }

   // struct stat st;
    if(x == 1){
        printf("showing stage\n");
        show_stage2(buffor,&xt);
                return 1;

    }
    else if(x==2){
        
        printf("write  stage\n");
        getchar();
        write_stage3(buffor,&xt);
                return 1;

    }
    else if(x==3){
        
        printf("walking stage\n");
        
        walk_stage4(buffor,&xt);
        return 1;
    }
    return 1;
 }

int main()
{
    while (interface_stage1())
        ;
    return EXIT_SUCCESS;
}

