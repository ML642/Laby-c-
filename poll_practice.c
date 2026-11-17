#define _POSIX_C_SOURCE 200809L

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

#include <linux/input.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

int main(int argc ,char** argv){
    if(argc < 2 ){
        ERR("usage");
    }
    struct pollfd fds[argc-1];
    for(int i = 1; i < argc ;i++){
        int fd = open(argv[i],O_NONBLOCK | O_RDONLY);

        if(fd == -1){
            ERR("open");
        }
        
        struct stat st;
        if(fstat(fd,&st) == -1){
            ERR("fstat");
        }
      
            if(!S_ISFIFO(st.st_mode)){
                ERR("not a fifo");
            }
        
        fds[i-1].fd = fd;
        fds[i-1].events = POLLIN;
        fds[i-1].revents = 0;
    }


    char message[5][128];
    int pos[5] = {0};

    int active  = argc - 1; 

    while(active > 0){
    int t = poll(fds,argc-1,2000);

    if(t == -1){
        if (errno == EINTR)
        continue;
        ERR("poll");
        
    }
    if(t == 0){
        printf("Brak nowych pomiarow");
    }
    else{
      
        for(int i = 0;i < argc-1;i++){
            if(fds[i].fd == -1){
                continue;
            }
            if(fds[i].revents & (POLLERR)){
                ERR("open");
            }
            if (fds[i].revents & (POLLERR | POLLNVAL))
                 ERR("poll fd");
            

            if(fds[i].revents & (POLLIN | POLLHUP)){
                
                char buffor[128];

                while(1){   
                    int readden = read(fds[i].fd,buffor ,128);

                    
                    if(readden == -1){
                        if(errno == EAGAIN || errno == EWOULDBLOCK){
                            break;
                        }
                        if(errno == EINTR)
                             continue;
                        ERR("read");
                    }
                    if(readden == 0){
                        printf("Koniec odczytu");
                            if (close(fds[i].fd) == -1)
                            ERR("close");

                            fds[i].fd = -1;
                            active--;
                            break;
                    }
                    int p = pos[i];
                    for(int q = p ; q < p + readden;q++){
                        if(buffor[q-p] == '\n'){
                            message[i][pos[i]] = '\0';
                            printf("%s",message[i]);
                            pos[i] = 0;
                        }
                        else{
                        if(pos[i]>=127){
                            ERR("message too long");
                        }
                         message[i][pos[i]] = buffor[q-p];
                         pos[i]++;
                        }   
                    }                    
                }

            }
        }
    }

}
}
