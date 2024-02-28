#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    char databuf[50];
    int fd,ret;
    off_t off;
    //打开文件
    fd = open("./test_file",O_RDWR | O_APPEND);
    if(-1 == fd)
    {
        perror("open error:");
        return -1;
    }

    memset(databuf,0x35,sizeof(databuf));

    off = lseek(fd,1,SEEK_SET);
    ret = write(fd,databuf,4);
    if(-1 == ret)
    {
        perror("write error:");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}