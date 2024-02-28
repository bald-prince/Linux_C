#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(void)
{
    int ret;
    struct stat file_info;
    struct tm file_time;
    char time_str[100];
    //1.获取文件属性
    ret = stat("./text.txt",&file_info);
    if(-1 == ret)
    {
        perror("stat error");
        return -1;
    }
    //2.打印inode编号及文件大小
    printf("inode = %ld, file = %ld bytes.\n",file_info.st_ino,file_info.st_size);

    //3.判断文件类型
    if(S_ISREG(file_info.st_mode))
        printf("regular file\r\n");
    else if(S_ISDIR(file_info.st_mode))
        printf("directory\r\n");
    else if(S_ISCHR(file_info.st_mode))
        printf("character device\r\n");
    else if(S_ISBLK(file_info.st_mode))
        printf("block device\r\n");
    else if(S_ISFIFO(file_info.st_mode))
        printf("FIFO\r\n");
    else if(S_ISLNK(file_info.st_mode))
        printf("symbolic link\r\n");
    else if(S_ISSOCK(file_info.st_mode))
        printf("socket\r\n");

    //4.判断文件对其他用户是否具有可读可写权限
    //可读
    if(file_info.st_mode & S_IROTH)
        printf("read\r\n");
    else
        printf("can't read\r\n");
    //可写
        if(file_info.st_mode & S_IWOTH)
        printf("write\r\n");
    else
        printf("can't write\r\n");

    //5.获取文件的时间属性
    //打印文件最后访问时间
    localtime_r(&file_info.st_atim.tv_sec, &file_time);
    strftime(time_str, sizeof(time_str),"%Y-%m-%d %H:%M:%S", &file_time);
    printf("time of last access: %s\n", time_str);

    //打印内容最后被修改时间
    localtime_r(&file_info.st_mtim.tv_sec, &file_time);
    strftime(time_str, sizeof(time_str),"%Y-%m-%d %H:%M:%S", &file_time);
    printf("time of last modification: %s\n", time_str);

    ///打印文件状态最后改变的时间
    localtime_r(&file_info.st_ctim.tv_sec, &file_time);
    strftime(time_str, sizeof(time_str),"%Y-%m-%d %H:%M:%S", &file_time);
    printf("time of last status change: %s\n", time_str);
    
    return 0;
   
}