#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    int fd;
    /* 打开文件 */
    fd = open("./test_file", O_WRONLY | O_TRUNC);
    if (-1 == fd) 
    {
        perror("open error");
        exit(-1);
    }
    /* 关闭文件 */
    close(fd);
    exit(0);
}