#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
int main(void)
{
    unsigned char buffer[100];
    int fd;
    int ret;
    /* 打开文件 test_file */
    fd = open("./test_file", O_RDWR);
    if (-1 == fd) 
    {
        perror("open error");
        exit(-1);
    }
    /* 使用 pread 函数读取数据(从偏移文件头 1024 字节处开始读取) */
    ret = pread(fd, buffer, sizeof(buffer), 1024);
    if (-1 == ret) 
    {
        perror("pread error");
        goto err;
    }
    /* 获取当前位置偏移量 */
    ret = lseek(fd, 0, SEEK_CUR);
    if (-1 == ret) 
    {
        perror("lseek error");
        goto err;
    }
    printf("Current Offset: %d\n", ret);
    ret = 0;
    err:
        /* 关闭文件 */
        close(fd);
        exit(ret);
}