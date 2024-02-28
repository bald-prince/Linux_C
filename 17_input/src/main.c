#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

int main(int argc, char *argv[])
{
    struct input_event in_ev;
    int x, y;  //触摸点x和y坐标
    int down;  //用于记录ABS_MT_TRACKING_ID事件的value,0表示按下,-1表示松开,-8表示移动
    int valid; //用于记录数据是否有效(我们关注的信息发生更新表示有效,1表示有效,0表示无效)
    int fd = -1;

    /* 校验传参 */
    if (2 != argc)
    {
        fprintf(stderr, "usage: %s <input-dev>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 打开文件 */
    if (0 > (fd = open(argv[1], O_RDONLY)))
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    x = y = 0; //初始化x和y坐标值
    down = -8; //初始化<移动>
    valid = 0; //初始化<无效>
    for (;;)
    {

        /* 循环读取数据 */
        if (sizeof(struct input_event) !=
            read(fd, &in_ev, sizeof(struct input_event)))
        {
            perror("read error");
            exit(EXIT_FAILURE);
        }

        switch (in_ev.type)
        {
        case EV_ABS: //绝对位移事件

            if (ABS_MT_TRACKING_ID == in_ev.code)
            {
                down = in_ev.value;
                valid = 1;
            }

            switch (in_ev.code)
            {
            case ABS_MT_POSITION_X: //X坐标
                x = in_ev.value;
                valid = 1;
                break;
            case ABS_MT_POSITION_Y: //Y坐标
                y = in_ev.value;
                valid = 1;
                break;
            }
            break;

        case EV_SYN: //同步事件
            if (SYN_REPORT == in_ev.code)
            {
                if (valid)
                { //判断是否有效
                    printf("同步\n");
                    switch (down)
                    { //判断状态
                    case 0:
                        printf("按下(%d, %d)\n", x, y);
                        break;
                    case -1:
                        printf("松开\n");
                        break;
                    case -8:
                        printf("移动(%d, %d)\n", x, y);
                        break;
                    }

                    valid = 0; //重置valid
                    down = -8; //重置down
                }
            }
            break;
        }
    }
}

 /*struct input_event in_ev = {0};
    int fd;
    //传入参数校验
    if(2 != argc)
    {
        fprintf(stderr,"usage: %s <input-dev>\n",argv[0]);
        exit(-1);
    }

    //打开文件
    fd = open(argv[1],O_RDONLY);
    if(-1 == fd)
    {
        perror("Open error");
        exit(-1);
    }

    for(;;)
    {
        //循环读取数据
        if( sizeof(struct input_event) != read(fd,&in_ev,sizeof(struct input_event)))
        {
            perror("Read error");
            exit(-1);
        }

        if(EV_KEY == in_ev.type)            //按键事件
        {
            switch (in_ev.value)
            {
                case 0:
                    printf("code<%d>:松开\r\n",in_ev.code);
                break;
                case 1:
                    printf("code<%d>:按下\r\n",in_ev.code);
                break;
                case 2:
                    printf("code<%d>:长按\r\n",in_ev.code);
                break;
            }
        }

        printf("type:%d code:%d value:%d\r\n",in_ev.type,in_ev.code,in_ev.value);
    }

    return 0;*/  