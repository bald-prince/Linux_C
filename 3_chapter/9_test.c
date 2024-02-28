#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
    int fd;
    /* 打开 file1 文件 */
    if (0 > (fd = open("./file1", O_RDWR))) 
    {
        perror("open error");
        exit(-1);
    }
    /* 使用 ftruncate 将 file1 文件截断为长度 0 字节 */
    if (0 > ftruncate(fd, 0)) 
    {
        perror("ftruncate error");
        exit(-1);
    }
    /* 使用 truncate 将 file2 文件截断为长度 1024 字节 */
    if (0 > truncate("./file2", 10)) 
    {
        perror("truncate error");
        exit(-1);
    }
    /* 关闭 file1 退出程序 */
    close(fd);
    exit(0);
}