#ifndef __UART_H
#define __UART_H
#define _GNU_SOURCE //在源文件开头定义_GNU_SOURCE 宏
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <termios.h>


typedef struct uart_hardware_cfg{
    unsigned int baudrate;          //波特率
    unsigned char dbit;             //数据位
    char parity;                    //奇偶校验位
    unsigned char sbit;             //停止位
}uart_cfg_t;

extern int uart_fd;

int uart_init(const char *device);
int uart_cfg(const uart_cfg_t *cfg);
void show_help(const char *app);
void io_handler(int sig,siginfo_t *info,void *context);
void async_io_init(void);
int close_uart(void);
#endif
