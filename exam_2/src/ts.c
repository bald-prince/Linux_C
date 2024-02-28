#include "ts.h"

int ts_fd;		            //static的作用是，不让这个变量在其他文件使用

//1、打开触摸屏文件
int open_ts(void)
{
	ts_fd = open("/dev/input/event1", O_RDONLY);
	if (ts_fd == -1)
	{
		printf("open event0 fail!\n");
		return -1;
	}
	return 0;
}

//2、读取触摸屏的xy的数据
int get_ts_xy(int *ts_x, int *ts_y)
{
	struct input_event ts_buf;
	int count = 0;
	int ts_x_last,ts_y_last;

	//2、读取触摸屏的数据
	while(1)
	{
		read(ts_fd, &ts_buf, sizeof(ts_buf));	//scanf函数
		//获取x轴坐标值
		if (ts_buf.type == EV_ABS && ts_buf.code == ABS_X)
		{
			*ts_x  = ts_buf.value/1024.0*800;	//黑色边
		}

		//获取y轴坐标值
		if (ts_buf.type == EV_ABS && ts_buf.code == ABS_Y)
		{
			*ts_y = ts_buf.value/600.0*480;	//黑色边
		}
		//判断你的手指，是否按压到了屏幕
		if(ts_buf.type == EV_KEY && ts_buf.code == BTN_TOUCH)
		{
			if(ts_buf.value == 1)  //手指按下时刻的坐标值
			{
				ts_x_last = *ts_x;  //记录下先前的坐标
				ts_y_last = *ts_y;
			}
			if(ts_buf.value == 0)
			{
				break;
			}
		}
	}
}

void get_xy(int *x, int *y)
{
	*x=-1;
	*y=-1;
	//2.读取数据
	struct input_event data;
	while(1)
	{
		read(ts_fd, &data, sizeof(data));

		if(data.type == 3 && data.code == 0)//x坐标
		{
			*x = data.value;
		}
		else if(data.type == 3 && data.code == 1)//y坐标
		{	
			*y = data.value;
			if(*x >= 0)
			{
				break;
			}
		}
	}
	
	//把1024*600的坐标换算800*480坐标
	//*x=(*x)*800/1024;
	//*y=(*y)*480/600;
}

//3、关闭触摸屏文件
int close_ts()
{
	close(ts_fd);
	return 0;
}
