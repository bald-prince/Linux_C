#include "ts.h"

int ts_fd;		            //static的作用是，不让这个变量在其他文件使用

//1、打开触摸屏文件
int open_ts(void)
{
	ts_fd = open(ts_path, O_RDONLY);
	if (-1 == ts_fd)
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
		read(ts_fd, &data, sizeof(data));

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


//3、关闭触摸屏文件
int close_ts()
{
	close(ts_fd);
	return 0;
}