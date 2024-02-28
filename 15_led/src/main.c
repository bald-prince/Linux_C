#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "led.h"

int main(int argc,char *argv[])
{
    //文件描述副
    int fd1,fd2;

    //判断传入参数个数
    if(2 > argc)
    {
        USAGE();
        return -1;
    }

    //打开文件
    fd1 = open(LED_TRIGGER,O_RDWR);
    if(-1 == fd1)
    {
        perror("Open error");
        return -1;
    }
    //打开文件
    fd2 = open(LED_BRIGHTNESS,O_RDWR);
    if(-1 == fd2)
    {
        perror("Open error");
        return -1;
    }

    if(!strcmp(argv[1],"on"))
    {
        write(fd1,"none",4);
        write(fd2,"1",1);
    }
    else if(!strcmp(argv[1],"off"))
    {
        write(fd1,"none",4);
        write(fd2,"0",1);
    }
    else if(!strcmp(argv[1],"trigger"))
    {
        if(3 != argc)
        {
            USAGE();
            return -1;
        }
        if(0 > write(fd1,argv[2],strlen(argv[2])))
            perror("Write error");
    }
    else
    {
        USAGE();
    }
    
    
    return 0;
}