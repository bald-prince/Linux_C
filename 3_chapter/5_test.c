#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int fd1,fd2,fd3;

    fd1 = open("./test_file",O_RDWR);
    if(-1 == fd1)
    {
        perror("Open error:");
        return -1;
    }

    fd2 = open("./test_file",O_RDWR);
    if(-1 == fd2)
    {
        perror("Open error:");
        return -1;
    }

    fd3 = open("./test_file",O_RDWR);
    if(-1 == fd3)
    {
        perror("Open error:");
        return -1;
    }

    printf("%d  %d  %d\r\n",fd1,fd2,fd3);
    close(fd1);
    close(fd2);
    close(fd3);
    return 0;
}