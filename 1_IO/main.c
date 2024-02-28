#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc,char *argv[])
{
    int fd1,fd2,ret1,ret2;
    char databuf[100] = {0};

    //运行必须三个参数
    if(argc != 3)
    {
        printf("Error Usage!\r\n");
        return -1;
    }

    //打开源文件
    fd1 = open(argv[1],O_RDWR);
    if(-1 == fd1)
    {
        printf("Open src_file failed!\r\n");
        return -1;
    }
    else
    {
        //读源文件
        ret1 = read(fd1,databuf,11);
        if(-1 == ret1)
        {
            printf("Read src_file failed\r\n");
            close(fd1);
            return -1;
        }
        else
        {
            //打开目标文件
            fd2 = open(argv[2],O_RDWR);
            if(-1 == fd1)
            {
                printf("Open src_file failed!\r\n");
                return -1;
            }
            else
            {
                //写入数据
               ret2 = write(fd2,databuf,11);
               if(-1 == ret2)
               {
                   printf("Write target_file failed!\r\n");
                   close(fd2);
                   return -1;
               }
               else
               {
                   printf("Wirte target_file success!\r\n");
               }
               
            }
              
        } 
    }

    //计算文件大小
    fd2 = open(argv[2],O_RDWR);
    if(-1 == fd1)
    {
        printf("Open src_file failed!\r\n");
        return -1;
    }
    else
    {
        off_t off = lseek(fd2,0,SEEK_END);
        printf("Size of the file is:%ld bytes!\r\n",off);
    }
    
    close(fd1);
    close(fd2);

    return 0;
}

/*int main(void)
{
    char databuf[] = {0};
    int fd,ret;
    char *filename = "./test.txt";
    
    fd = open(filename,O_RDWR | O_CREAT | O_EXCL, S_IROTH | S_IRGRP | S_IRUSR | S_IWUSR);
    if(-1 == fd)
    {
        printf("Open file failed!\r\n");
        return -1;
    }
    else
    {
        ret = write(fd,"Hello World",sizeof("Hello World") - 1);
        if(-1 == ret)
        {
            printf("Wirte data failed!\r\n");
            close(fd);
            return -1;
        }
        else
        {
            printf("str have %ld bytes,write %d bytes\r\n",sizeof("Hello World") - 1,ret);
            read(fd,databuf,sizeof("Hello World"));
            printf("data = %s\r\n",databuf);
        } 
    }
    close(fd);
    return 0;
}*/

