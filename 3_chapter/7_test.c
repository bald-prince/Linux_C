#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void)
{
    int fd1,fd2,ret;
    char databuf[20] = {0};

    //打开文件
    fd1 = open("./test_file",O_RDWR | O_APPEND);
    if(-1 == fd1)
    {
        perror("Open error:");
        return -1;
    }

    //复制文件
    fd2 = dup2(fd1,1000);
    printf("fd2 = %d\r\n",fd2);
    if(-1 == fd2)
    {
        perror("Dup error:");
        close(fd1);
        return -1;
    }
    //读取复制后的文件数据
    ret = read(fd2,databuf,11);
    if(-1 == ret)
    {
        perror("Read error:");
        close(fd1);
        close(fd2);
        return -1;
    }
    else
    {
        //打印数据
        printf("data = %s\r\n",databuf);
        //写入数据
        write(fd2," HFUT",5);

    }
    close(fd1);
    close(fd2);
    return 0;
}