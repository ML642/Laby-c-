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
#include <sys/uio.h>
#include <sys/mman.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))


int main(int argc, char** argv){
    if(argc < 2){
        ERR("usage");
    }
    char* dest = argv[1];

    int fd = open(dest,O_RDONLY);

    if(fd == -1){
        ERR("open");
    }
    struct stat st;

    if(fstat(fd,&st) == -1){
        ERR("fstat");
    }
    if (st.st_size == 0) {
        printf("File size: 0 bytes\nNumber of 'a': 0\n");

        if (close(fd) == -1)
            ERR("close");

        return EXIT_SUCCESS;
    }

    char* buff = mmap(NULL,st.st_size,PROT_READ, MAP_SHARED,fd,0);

    
    if(buff == MAP_FAILED){
        ERR("mmap");
    }
    
    int count = 0;
    for(int i = 0 ; i < st.st_size ; i++){
        if(buff[i] == 'a')count++;
    }
    printf("%ld \n %d\n ",st.st_size,count);


    if(munmap(buff,st.st_size) == -1){
        ERR("mmap");
    }
    if(close(fd) == -1){
        ERR("close");
    }
    
    return EXIT_SUCCESS;
}