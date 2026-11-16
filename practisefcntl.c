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

void usage(){
    printf("%s","program -m1");
    return;
}

void setIO(int number){

    int ag = fcntl(STDIN_FILENO,F_GETFL);

    if(ag == -1){
        ERR("dfasdfad");    
    }

    if(number==1)ag |= O_NONBLOCK;
    else{
        ag &= ~O_NONBLOCK;
    }

    if(fcntl(STDIN_FILENO,F_SETFL,ag)==-1){
        ERR("err");
    }
}

int main(void)
{
    setIO(1);

    char buff[101];

    int readd = read(STDIN_FILENO,buff,100);

    if(readd == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            printf("Brak danych\n");
            return EXIT_SUCCESS;
        }
         ERR("read");
    }

    if(readd == 0){
        printf("EOF");
        return EXIT_SUCCESS;
    }
    
    buff[readd] = '\0';
    printf("%s",buff);

}