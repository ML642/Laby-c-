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
// #include <sys/uio.h>
// #include <sys/mman.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

int main(int argc, char** argv){
    int fd = open("numbers.txt",O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd == -1){
        ERR("open");
    }
    ssize_t written = 0;
    char* buf = "1234567890";
    ssize_t len = strlen(buf);
    
    while(written < len){
        ssize_t res = write(fd,buf+written,len-written);

        if(res == -1){
            if(errno == EINTR){
                continue;
            }
            ERR("write");
        }
        if(res == 0){
            fprintf(stderr, "write made no progress\n");
            exit(EXIT_FAILURE);
        }
        written += res;  
    }

    if(close(fd) == -1){
        ERR("close");
    }
    return EXIT_SUCCESS;
}