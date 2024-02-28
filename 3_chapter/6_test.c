#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    char buffer[4];
    int fd1, fd2;
    int ret;
    /*创建新文件 test_file 并打开 */
    fd1 = open("./test_file", O_RDWR | O_TRUNC | O_APPEND);
    if (-1 == fd1) 
    {
        perror("open error");
        return -1;
    }
    /* 再次打开 test_file 文件 */
    fd2 = open("./test_file", O_RDWR);
    if (-1 == fd2) 
    {
        perror("open error");
        close(fd1);
        return -1;
    }
    /* 通过 fd1 文件描述符写入 4 个字节数据 */
    buffer[0] = 0x11;
    buffer[1] = 0x22;
    buffer[2] = 0x33;
    buffer[3] = 0x44;
    ret = write(fd1, buffer, 4);
    if (-1 == ret) 
    {
        perror("write error");
        close(fd1);
        close(fd2);
        return -1;
    }
    /* 将读写位置偏移量移动到文件头 */
    ret = lseek(fd2, 0, SEEK_SET);
    if (-1 == ret) 
    {
        perror("lseek error");
        close(fd1);
        close(fd2);
        return -1;
    }
    /* 读取数据 */
    memset(buffer, 0x00, sizeof(buffer));
    ret = read(fd2, buffer, 4);
    if (-1 == ret) 
    {
        perror("read error");
        close(fd1);
        close(fd2);
        return -1;
    }
    printf("0x%x 0x%x 0x%x 0x%x\n", buffer[0], buffer[1],buffer[2], buffer[3]);

    close(fd1);
    close(fd2);
    return 0;

}