#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

int main(void)
{
    if (mkfifo("test_fifo", 0666) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }
    }

    return 0;
}