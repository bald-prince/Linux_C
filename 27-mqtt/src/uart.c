#include "uart.h"

/*struct termios old_cfg;         //用于保存终端的配置参数
int uart_fd = -1;

//串口初始化
int uart_init(const char *device)
{
    //打开串口
    uart_fd = open(device,O_RDWR | O_NOCTTY);
    if(-1 == uart_fd)
    {
        perror("Open uart error");
        return -1;
    }

    //获取串口当前配置参数
    if(0 > tcgetattr(uart_fd,&old_cfg))
    {
        perror("tcgetattr error");
        close(uart_fd);
        return -1;
    }
    return 0;
}

//关闭串口
int close_uart(void)
{
    tcsetattr(uart_cfg,TCSANOW,&old_cfg);       //恢复之前的配置
    close(uart_fd);
    return 0;
}

//串口配置
int uart_cfg(const uart_cfg_t *cfg)
{
    struct termios new_cfg = {0};
    speed_t speed;

    //设置为原始模式
    cfmakeraw(&new_cfg);

    //使能接收
    new_cfg.c_cflag |= CREAD;

    //使能波特率
    switch (cfg->baudrate)
    {
        case 1200:
            speed = B1200;
            break;
        case 1800:
            speed = B1800;
            break;
        case 2400:
            speed = B2400;
            break;
        case 4800:
            speed = B4800;
            break;
        case 9600:
            speed = B9600;
            break;
        case 19200:
            speed = B19200;
            break;
        case 38400:
            speed = B38400;
            break;
        case 57600:
            speed = B57600;
            break;
        case 115200:
            speed = B115200;
            break;
        case 230400:
            speed = B230400;
            break;
        case 460800:
            speed = B460800;
            break;
        case 500000:
            speed = B500000;
        break;
        default:
            speed = B115200;
            printf("default baud rate:115200\n");
            break;
    }

    if(0 > cfsetspeed(&new_cfg,speed))
    {
        perror("cfsetspeed error");
        return -1;
    }

    //设置数据位大小
    new_cfg.c_cflag &= ~CSIZE;      //数据位相关比特清0
    switch (cfg->dbit)
    {
        case 5:
            new_cfg.c_cflag |= CS5;
            break;
        case 6:
            new_cfg.c_cflag |= CS6;
            break;
        case 7:
            new_cfg.c_cflag |= CS7;
            break;
        case 8:
            new_cfg.c_cflag |= CS8;
            break;
        default:
            new_cfg.c_cflag |= CS8;
            printf("default data bit size:8\n");
            break;
    }

    //设置奇偶校验位
    switch (cfg->parity)
    {
        case 'N':       //无校验
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            break;
        case 'O':       //奇校验
            new_cfg.c_cflag |= (PARODD|PARENB);
            new_cfg.c_iflag |= INPCK;
            break;
        case 'E':       //偶校验
            new_cfg.c_cflag |= PARENB;
            new_cfg.c_cflag &= PARODD;
            new_cfg.c_iflag |= INPCK;
            break;        
        default:
            new_cfg.c_cflag &= ~PARENB;
            new_cfg.c_iflag &= ~INPCK;
            printf("default parity:N\n");
            break;
    }

    //设置停止位
    switch (cfg->sbit)
    {
        case 1:         //1位停止位
            new_cfg.c_cflag &= ~CSTOPB;
            break;
        case 2:         //2位停止位
            new_cfg.c_cflag |= CSTOPB;
            break;
        default:
            new_cfg.c_cflag &= ~CSTOPB;
            printf("default stop bit size:1\n");
            break;
    }

    //将MIN和TIME设置为0
    new_cfg.c_cc[VTIME] = 0;
    new_cfg.c_cc[VMIN] = 0;

    //清空缓冲区
    if(0 > tcflush(uart_fd,TCIOFLUSH))
    {
        perror("tcflush error");
        return -1;
    }

    //写入配置
    if(0 > tcsetattr(uart_fd,TCSANOW,&new_cfg))
    {
        perror("tcserattr error");
        return -1;
    }

    return 0;
}


// --dev=/dev/ttymxc2
// --brate=115200
// --dbit=8
// --parity=N
// --sbit=1
// --type=read
//打印帮助信息
void show_help(const char *app)
{
    printf("Usage: %s [选项]\n"
            "\n 必选选项:\n"
            " --dev=DEVICE 指定串口终端设备名称, 譬如--dev=/dev/ttymxc2\n"
            " --type=TYPE 指定操作类型, 读串口还是写串口, 譬如--type=read(read 表示读、 write 表示写、其它值无效)\n"
            "\n 可选选项:\n"
            " --brate=SPEED 指定串口波特率, 譬如--brate=115200\n"
            " --dbit=SIZE 指定串口数据位个数, 譬如--dbit=8(可取值为: 5/6/7/8)\n"
            " --parity=PARITY 指定串口奇偶校验方式, 譬如--parity=N(N 表示无校验、 O 表示奇校验、 E 表示偶校验)\n"
            " --sbit=SIZE 指定串口停止位个数, 譬如--sbit=1(可取值为: 1/2)\n"
            " --help 查看本程序使用帮助信息\n\n", app);
}

//信号处理函数，当串口有数据可读时，会跳转到该函数执行
void io_handler(int sig,siginfo_t *info,void *context)
{
    unsigned char buf[10] = {0};
    int ret,n;
    if(SIGRTMIN != sig)
        return;

    //判断串口是否有数据可读
    if(POLL_IN == info->si_code)
    {
        ret = read(uart_fd,buf,8);      //一次最多读8个字节数据
        printf("[");
        for(n = 0;n < ret;n++)
            printf("0x%hhx",buf[n]);
        printf("]");
    }
}

//异步I/O初始化函数
void async_io_init(void)
{
    struct sigaction sigatn;
    int flag;

    //使能异步I/O
    flag = fcntl(uart_fd,F_GETFL);      //使能串口的异步I/O功能
    flag |= O_ASYNC;
    fcntl(uart_fd,F_SETFL,flag);

    //设置异步I/O的所有者
    fcntl(uart_fd,F_SETOWN,getpid());

    //指定实时信号SIGRTMIN作为异步I/O通知信号
    fcntl(uart_fd,F_SETSIG,SIGRTMIN);

    //为实时信号SIGRTMIN注册信号处理函数
    sigatn.sa_sigaction = io_handler;       //串口有数据可读，跳转到io_handler函数
    sigatn.sa_flags = SA_SIGINFO;
    sigemptyset(&sigatn.sa_mask);
    sigaction(SIGRTMIN,&sigatn,NULL);
}*/