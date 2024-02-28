#include "ts.h"

int fd;		            //static的作用是，不让这个变量在其他文件使用

//1、打开触摸屏文件
int open_ts(void)
{
	fd = open(ts_path, O_RDONLY);
	if (-1 == fd)
	{
		perror("Open error");
		return -1;
	}
	return 0;
}

//2.获取触摸屏x、y坐标
void get_xy(int *x, int *y)
{
	*x = 0;
	*y = 0;
	struct input_event data;

	while(1)
	{
		read(fd, &data, sizeof(data));

		if(data.type == EV_ABS && data.code == ABS_MT_POSITION_X)//x坐标
			*x = data.value;
		
		else if(data.type == EV_ABS && data.code == ABS_MT_POSITION_Y)//y坐标
		{	
			*y = data.value;
			if(*x >= 0)
				break;
		}
	}
}

void get_xy_tslib(int *x,int *y)
{
	*x = 0;
	*y = 0;
	struct tsdev *ts = NULL;
	struct ts_sample samp;

	//打开并配置触摸屏设备
	ts = ts_setup(NULL, 0);
	if (NULL == ts) 
		fprintf(stderr,"ts_setup error");

	while(1)
	{
		ts_read(ts, &samp, 1);
		*x = samp.x;
		*y = samp.y;

		if(*x >= 0)
			break;
	}
}


//3、关闭触摸屏文件
int close_ts()
{
	close(fd);
	return 0;
}

void touch_event_handler(void) 
{
    int x, y;

    while(1) 
	{
        get_xy(&x, &y);

        if(x <= 340  && x >=0 && y <= 600 && y >= 0)
		{
            led_switch("on");
			my_ui();
            v4l2_state = 0; // 将退出标志设为0，请求停止 v4l2 线程
        } 
		else if(x <= 680  && x >= 341 && y <= 600 && y >= 0)
		{
            led_switch("off");
			my_ui();
            v4l2_state = 0; 
        } 
		else if(x <= 1024  && x >= 681 && y <= 600 && y >= 0) 
		{
            v4l2_state = 1; // 设置全局标志以开始运行 v4l2 线程
            pthread_create(&v4l2_thread, NULL, show_video, NULL); // 创建新线程
        }
    }
}