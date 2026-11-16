#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <linux/input.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))



int main(int argc, char** argv){
    
    struct pollfd fds[1];
   
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;

    int res = poll(fds,1,3000);

    if(res == -1){
        ERR("poll");
    }
    if(res == 0){
        ERR("TIMEOUT\n");
    }
    if((fds[0].revents & POLLHUP) || ( fds[0].revents & POLLERR)){
        ERR("revents\n");
    }

    if(fds[0].revents & POLLIN){
        char buff[1001];
        int read1 = read(STDIN_FILENO,buff,1000);
        if(read1 == -1){
            ERR("read1");
        } 
        buff[read1] = '\0';
        printf("%s YEAH",buff);
    }
    else{
        printf("NOO\n");
    }
}