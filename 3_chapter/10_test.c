#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
    int fd,ret;
    char databuf[11] = "hello world";
    char buf[2000] = {0};
    fd = open("./text.txt",O_RDWR);
    if(-1 == fd)
    {
        perror("open error");
        return -1;
    }
    //for (int i = 0;i < 5;i++)
    //    write(fd,databuf,sizeof(databuf));

    ret = read(fd,buf,50);
    printf("ret = %d,databuf = %ld\r\n",ret,sizeof(buf));


    close(fd);
    return 0;

}