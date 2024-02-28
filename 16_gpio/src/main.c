#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "gpio.h"
#include <linux/input.h>
#include <poll.h>

int main(int argc,char *argv[])
{
    printf("mode 1:gpio out\r\nmode 2:gpio in\r\nmode 3:gpio exit!\r\n");
    printf("gpio out usage: %s <mode> <gpio> <value>\r\n",argv[0]);
    printf("gpio in usage: %s <mode> <gpio>\r\n", argv[0]);
    int mode = 0;
    mode = atoi(argv[1]);

    if(mode == 1)       //输出模式
    {
        static char gpio_path[100];
        /* 校验传参 */
        if (4 != argc) 
        {
            fprintf(stderr,"gpio out usage: %s <mode> <gpio> <value>\n", argv[0]);
            exit(-1);
        }

        //判断gpio编号是否已导出
        sprintf(gpio_path,"/sys/class/gpio/gpio%s",argv[2]);

        if(access(gpio_path,F_OK))      //检查是否存在
        {
            int fd,len;
            fd = open("/sys/class/gpio/export",O_WRONLY);
            if(-1 == fd)
            {
                perror("Open error");
                return -1;
            }
            len = strlen(argv[2]);
            if(len != write(fd,argv[2],len))
            {
                perror("Write error");
                close(fd);
                return -1;
            }
            close(fd);
        }

        //配置为输出
        if(gpio_config("direction","out",gpio_path))
            return -1;
            
        //极性设置
        if(gpio_config("active_low","0",gpio_path))
            return -1;

        //控制GPIO输出高低电平
        if(gpio_config("value",argv[3],gpio_path))
            return -1;
    
        return 0;
    }
    else if(mode == 2)          //输入模式
    {
        static char gpio_path[100];
        static char file_path[100];
        char val;
        int fd;
        //校验传参 
        if (3 != argc) 
        {
            fprintf(stderr, "gpio in usage: %s <mode> <gpio>\n", argv[0]);
            exit(-1);
        }
        
        //判断gpio编号是否已导出
        sprintf(gpio_path,"/sys/class/gpio/gpio%s",argv[2]);
        if(access(gpio_path,F_OK))      //检查是否存在
        {
            int fd,len;

            fd = open("/sys/class/gpio/export",O_WRONLY);
            if(-1 == fd)
            {
                perror("Open error");
                return -1;
            }

            len = strlen(argv[2]);
            if(len != write(fd,argv[2],len))
            {
                perror("Write error");
                close(fd);
                return -1;
            }

            close(fd);
        }

        //配置为输入
        if(gpio_config("direction","in",gpio_path))
            return -1;

        //极性设置
        if(gpio_config("active_low","0",gpio_path))
            return -1;

        //配置为非中断设置
        if(gpio_config("edge","none",gpio_path))
            return -1;

        //读取GPIO电平
        sprintf(file_path,"%s/%s",gpio_path,"value");

        fd = open(file_path,O_RDWR);
        if(-1 == fd)
        {
            perror("Open error");
            return -1;
        }

        if(0 > read(fd,&val,1))
        {
            perror("Read error");
            close(fd);
            return -1;
        }

        printf("value: %c\r\n",val);

        close(fd);
        return 0;
    }
    else if(mode == 3)                       
    {
        struct pollfd pfd;
        static char gpio_path[100];
        static char file_path[100];
        int ret;
        char val;

        //校验传递参数个数
        if(3 != argc)
        {
            fprintf(stderr,"gpio exit usage: %s <mode> <gpio>\n",argv[0]);
            return -1;
        }

        //判断gpio编号是否已导出
        sprintf(gpio_path,"/sys/class/gpio/gpio%s",argv[2]);
        if(access(gpio_path,F_OK))      //检查是否存在
        {
            int fd,len;

            fd = open("/sys/class/gpio/export",O_WRONLY);
            if(-1 == fd)
            {
                perror("Open error");
                return -1;
            }

            len = strlen(argv[2]);
            if(len != write(fd,argv[2],len))
            {
                perror("Write error");
                close(fd);
                return -1;
            }

            close(fd);
        }

        //配置为输入模式
        if(gpio_config("direction","in",gpio_path))
            return -1;

        //极性设置
        if(gpio_config("active_low","0",gpio_path))
            return -1;

        //配置为中断触发模式，上升沿、下降沿
        if(gpio_config("edge","both",gpio_path))
            return -1;

        //打开value属性文件
        sprintf(file_path,"%s/%s",gpio_path,"value");

        pfd.fd = open(file_path,O_RDWR);
        if(-1 == pfd.fd)
        {
            perror("Open error");
            return -1;
        }

        //调用poll
        pfd.events = POLLPRI;       //只关心高优先级数据可读

        read(pfd.fd,&val,1);        //先读取一次清除状态
        for(;;)
        {
            ret = poll(&pfd,1,-1);  //调用poll
            if(0 > ret)
            {
                perror("Poll error");
                return -1;
            }
            else if(0 == ret)
            {
                fprintf(stderr,"poll timeout.\n");
                continue;
            }

            //检验高优先级数据是否可读
            if(pfd.revents & POLLPRI)
            {
                if(0 > lseek(pfd.fd,0,SEEK_SET))
                {
                    perror("Lseek error");
                    return -1;
                }
                if(0 > read(pfd.fd,&val,1))
                {
                    perror("Read error");
                    return -1;
                }

                printf("GPIO 中断触发<value = %c>\r\n",val);
            }
        }

        return 0;
    }
    else
    {
        printf("gpio out usage: %s <gpio> <value>\n",argv[0]);
        printf("gpio in usage: %s <gpio>\n", argv[0]);
        return 0;
    }
    
}