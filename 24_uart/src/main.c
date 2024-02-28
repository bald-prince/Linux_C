#include "ts.h"
#include "led.h"
#include "lcd.h"
#include "camera.h"
#include "display.h"
#include <unistd.h>
#include "function.h"
#include "uart.h"

int main(int argc,char *argv[])
{
    uart_cfg_t cfg = {0};
    char *device = NULL;
    int rw_flag = -1;

    unsigned char w_buf[10] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};        //通过串口发送出去的数据
    int n;

    for(n = 1; n < argc; n++)
    {
        if(!strncmp("--dev=",argv[n],6))
            device = &argv[n][6];
        else if(!strncmp("--brate=",argv[n],8))
            cfg.baudrate = atoi(&argv[n][8]);
        else if(!strncmp("--dbit=",argv[n],7))
            cfg.dbit = atoi(&argv[n][7]);
        else if(!strncmp("--parity=",argv[n],9))
            cfg.parity = atoi(&argv[n][9]);  
        else if(!strncmp("--sbit=",argv[n],7))
            cfg.sbit = atoi(&argv[n][7]); 
        else if(!strncmp("--type=",argv[n],7))
        {
            if(!strcmp("read",&argv[n][7]))
                rw_flag = 0;        //读
            else if(!strcmp("write",&argv[n][7]))
                rw_flag = 1;        //写
        }
        else if(!strcmp("--help",argv[n]))
        {
            show_help(argv[0]);        //打印帮助信息
            exit(EXIT_SUCCESS);
        }
    }

    if(NULL == device || -1 ==rw_flag)
    {
        fprintf(stderr, "Error: the device and read|write type must be set!\n");
        show_help(argv[0]);
        exit(EXIT_FAILURE);
    }

    //串口初始化
    if(uart_init(device))
        exit(EXIT_FAILURE);

    //串口配置
    if(uart_cfg(&cfg))
    {
        close_uart();
        exit(EXIT_FAILURE);
    }

    switch (rw_flag)
    {
        case 0:
            async_io_init();
            for( ; ; )
                sleep(1);
            break;
        case 1:
            for( ; ; )
            {
                write(uart_fd,w_buf,8);
                sleep(1);
            }  
            break;    
    }
    close_uart();
    exit(EXIT_SUCCESS);

}
