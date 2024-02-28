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