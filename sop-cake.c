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
#include <poll.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void usage(){
    printf("./program recipe granny1 granny2");
    exit(EXIT_FAILURE);
}

int main(int argc,char** argv){
    if(argc < 3 || argc > 7){
        usage();
    }
    int arr[50];
    char messages[5][128];
    int positions[5] = {0};
    
    
    for(int i = 2; i < argc ;i++){
        arr[i] = open(argv[i],O_RDONLY | O_NONBLOCK);
        if(arr[i] == -1){
            ERR("open");
        }
        struct stat fifo_st;

        if (fstat(arr[i], &fifo_st) == -1)
            ERR("fstat");

        if (!S_ISFIFO(fifo_st.st_mode)) {
            fprintf(stderr, "Nie ma babci na wieży %s\n", argv[i]);
            exit(EXIT_FAILURE);
        }    
    }
    int active = argc - 2;
    struct pollfd fds[argc - 2];

    for(int i = 0 ; i < argc-2;i++){
        fds[i].fd = arr[i+2];
        fds[i].events = POLLIN;
        fds[i].revents = 0;
    }
     int ex = 1;
    struct stat st;
    if(lstat(argv[1],&st) == -1){
        if(errno == ENOENT){
            ex = 0;
        }
        else{
            ERR("lstat");
        }
    }
    int fd = open( argv[1], O_RDWR | O_CREAT , 0644 );
    if(fd == -1){
                ERR("open");
            }
    if(ex == 0){
            
            char s[64];
            for(int i = 0 ; i < 63;i++){
                s[i] = ' '; 
            }  
            s[63] = '\n';
            
            for(int i = 0; i < 32 ;i++){
                int writen = write(fd,s,64);
                if(writen == -1 || writen < 64){
                    ERR("write");
                }
            }
    }
    int size = 0;
    if((size=lseek(fd,0,SEEK_END)) == -1){
        ERR("lseek");
    }
    if(size != 2048){
        fprintf(stderr,"Ta ksiega to nie przepis!");
        return EXIT_FAILURE;    
    }
     while (active > 0) {
            int ready = poll(fds, argc - 2, 3000);

            if (ready == -1) {
                if (errno == EINTR)
                    continue;
                ERR("poll");
            }

            if (ready == 0) {
                printf("Timeout\n");
                continue;
            }

            for (int i = 0; i < argc - 2; i++) {
                if (fds[i].fd == -1)
                    continue;

                if (fds[i].revents & (POLLIN | POLLHUP)) {
                     char buff[128];
            ssize_t rd = read(fds[i].fd, buff, sizeof(buff));

            if (rd == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK ||
                    errno == EINTR)
                    continue;
                ERR("read");
            }

            if (rd == 0) {
                if (close(fds[i].fd) == -1)
                    ERR("close");

                fds[i].fd = -1;
                active--;
                continue;
            }

            for (int k = 0; k < rd; k++) {
                if (buff[k] == '\n') {
                    int valid = 1;
                    int number = 0;

                    if (positions[i] < 3 ||
                        messages[i][2] != ':' ||
                        messages[i][0] < '0' ||
                        messages[i][0] > '9' ||
                        messages[i][1] < '0' ||
                        messages[i][1] > '9') {
                        valid = 0;
                    } else {
                        number = (messages[i][0] - '0') * 10
                               + (messages[i][1] - '0');

                        if (number > 31)
                            valid = 0;
                    }

                    if (valid) {
                        char line[64];

                        for (int j = 0; j < 63; j++) {
                            if (j < positions[i] - 3)
                                line[j] = messages[i][j + 3];
                            else
                                line[j] = ' ';
                        }

                        line[63] = '\n';

                        if (lseek(fd, number * 64, SEEK_SET) == -1)
                            ERR("lseek");
                        size_t written = 0;

                        while (written < sizeof(line)) {
                            ssize_t wr = write(
                                fd,
                                line + written,
                                sizeof(line) - written
                            );

                            if (wr == -1) {
                                if (errno == EINTR)
                                    continue;
                                ERR("write");
                            }

                            if (wr == 0) {
                                fprintf(stderr, "Nie udalo sie zapisac kroku\n");
                                exit(EXIT_FAILURE);
                            }

                            written += wr;
                        }

                        printf("Krok %02d zapisany\n", number);
                    } else {
                        fprintf(stderr, "Babcia sie zagadala\n");
                    }

                    positions[i] = 0;
                } else {
                    if (positions[i] < 127) {
                        messages[i][positions[i]] = buff[k];
                        positions[i]++;
                    } else {
                        fprintf(stderr, "Message too long\n");
                        exit(EXIT_FAILURE);
                    }
                }
             }
            }
        }
    }

    // int fd2 = open(argv[2],O_RDONLY);
    // if(fd2 == -1){
    //     if(errno == ENOENT){
    //         fprintf(stderr,"Nie ma babci na wieży %s",argv[2]);
    //     }
    //     return EXIT_FAILURE;
    // }
    // struct stat st2;
    // if(fstat(fd2,&st2) == -1){
    //     ERR("fstat");
    // }
    // if(!S_ISFIFO(st2.st_mode)){
    //     fprintf(stderr,"Nie ma babci na wieży X");
    //     return EXIT_FAILURE;
    // }  
    // char message[129];
    // int pos = 0;    
    // while (1) {
    //     char buff[128];
    //     ssize_t rd = read(fd2, buff, sizeof(buff));
    //     if (rd == 0)
    //         break;
    //     if (rd == -1)
    //         ERR("read");

            
    //     for(int i = 0 ; i < rd ; i++){
    //         if(buff[i] == '\n'){
    //                 int valid = 1;
    //                 int number = 0;

    //                 if (pos < 3 || message[2] != ':' ||
    //                     message[0] < '0' || message[0] > '9' ||
    //                     message[1] < '0' || message[1] > '9') {
    //                     valid = 0;
    //                 }
    //                 else {
    //                     number = (message[0] - '0') * 10
    //                         + (message[1] - '0');

    //                     if (number > 31)
    //                         valid = 0;
    //                 }
                    
                    
    //                 if (valid) {
    //                     char line[64];

    //                     for (int j = 0; j < 63; j++) {
    //                         if (j < pos - 3)
    //                             line[j] = message[j + 3];
    //                         else
    //                             line[j] = ' ';
    //                     }

    //                     line[63] = '\n';

    //                     if (lseek(fd, number * 64, SEEK_SET) == -1)
    //                         ERR("lseek");

    //                     if (write(fd, line, 64) != 64)
    //                         ERR("write");
    //                     else{   
    //                        printf("Krok %02d zapisany\n", number);
    //                     }
    //                 }
    //                 else{
    //                     fprintf(stderr, "Babcia sie zagadala\n");
    //                 }
    //                 pos = 0;
                    
    //         }
    //         else{
    //             if(pos < 127){
    //                 message[pos] = buff[i];
    //                 pos++;
    //             }
    //             else{
    //                 fprintf(stderr, "Message too long\n");
    //                 exit(EXIT_FAILURE);
    //             }
    //         }
    //     }
        
        
    // }

    char* buf = mmap(NULL, 2048 , PROT_READ | PROT_WRITE, MAP_SHARED,fd,0);

    if(buf == MAP_FAILED){
        ERR("MAP FAILED");
    }

    int count2 = 0;
    for(int i = 0; i < 2048 - 10; i++){
        if( buf[i] == 'r' &&
            buf[i+1] ==  'a' && 
            buf[i+2] ==  'w' &&
            buf[i+3] == '~' &&
            buf[i+4] == 'b' &&
            buf[i+5] == 'e' && 
            buf[i+6] == 'r' &&
            buf[i+7] == 'r' &&
            buf[i+8] == 'i' &&
            buf[i+9] == 'e' &&
            buf[i+10] == 's'){
                buf[i+3] = 's';
                buf[i+4] = 'p';
                count2 += 1;
        }
    }
    printf("Brudy księżniczki Joanny wyczyszczone: %d\n", count2);
    int false_count = 0;
    for(int i = 0 ; i < 32 ; i++){

        int  start = i * 64;

        char buf2[64];
        int count = 63;

        for (int j = 0; j < 63; j++) {
            buf2[j] = buf[start + j];
        }
        while (count > 0 && buf2[count - 1] == ' ') {
            count--;
        }
        if(count == 0){
                    false_count++;
                    continue;
                }

        char number[5];

        if(count>0) 
        {
            int n = snprintf(number, sizeof(number), "%02d: ", i);

        int wr2 = write(STDOUT_FILENO, number, n);
        if (wr2 == -1)
              ERR("write");
        }
        int wr = write(STDOUT_FILENO,buf2,count);
        if(wr == -1){
            ERR("write");
        }
        if (write(STDOUT_FILENO, "\n", 1) != 1)
          ERR("write");
    }

    if(munmap(buf,2048) == -1){
        ERR("munmap");
    }

    if(false_count > 0){
        printf("Brakuje %d kroków, tort sie zapadnie!\n", false_count);
    }
    else{
        printf("Tort gotowy!");
    }
    


    if(close(fd)){
        ERR("close");
    }
    // if(close(fd2)){
    //     ERR("close fd2");
    // }

}
