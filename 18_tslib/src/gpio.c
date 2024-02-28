#include "gpio.h"

int gpio_config(const char *attr,const char *val,char *gpio_path)
{
    char file_path[100];
    int len;
    int fd;

    sprintf(file_path,"%s/%s",gpio_path,attr);
    fd = open(file_path,O_RDWR);
    if(-1 == fd)
    {
        perror("Open error");
        return -1;
    }
    len = strlen(val);
    if(len != write(fd,val,len))
    {
        perror("Wrire error");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}