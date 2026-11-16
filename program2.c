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
// #include <sys/mman.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define MAX_BOOKS 100

typedef struct {
    unsigned int size;
    char title[65];
} Book;

int main(int argc, char** argv){
 
    if( argc < 3 ){
        ERR("usage");
    }
    int fd = open(argv[1],O_RDONLY);

    if(fd == -1){
        ERR("open");
    }
    struct stat st;
    if(fstat(fd,&st) == -1){
        ERR("stat");
    }

    if(st.st_size % 68 != 0){
        fprintf(stderr, "Invalid database size\n");
        exit(EXIT_FAILURE);
    }
    int N = st.st_size / 68;

    if(N > MAX_BOOKS){
        ERR("too many books");
    }
    
    Book books[MAX_BOOKS];
    struct iovec buffers[MAX_BOOKS];

    for(int i=0; i<N; i++){
        buffers[i].iov_base = &books[i];
        buffers[i].iov_len = 68;
        books[i].title[64] = '\0';
    }

    ssize_t n = readv(fd, buffers, N);
    if(n == -1){
        ERR("readv");
    }

    if (n != st.st_size) {
        fprintf(stderr, "Incomplete read\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++) {
        printf("Size: %u Title: %s\n",
            books[i].size,
            books[i].title);

        books[i].size++;
    }

    int fd2 = open(argv[2],O_WRONLY | O_CREAT | O_TRUNC , 0644);

    if(fd2 == -1){
        ERR("opp");
    }
    ssize_t written = writev(fd2, buffers, N);

    if (written == -1)      ERR("writev");

    if (written != st.st_size) {
        fprintf(stderr, "Incomplete write\n");
        exit(EXIT_FAILURE);
    }
    if(close(fd) || close(fd2)){
        ERR("err");
    }
}