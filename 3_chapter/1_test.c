#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


int main(void)
{
    int fd;
    /* 打开文件 */
    fd = open("./test_file", O_RDONLY);
    if (-1 == fd)
    {
        printf("Error: %s\n", strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}