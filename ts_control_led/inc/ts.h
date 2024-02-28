#ifndef __TS_H
#define __TS_H

/*
	1、头文件
	2、宏定义
	3、函数声明
	....
*/
#include <stdio.h>
//open、lseek函数
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//read、lseek、write、close函数
#include <unistd.h>
//ts的头文件
#include <linux/input.h>
//usleep函数
#include <unistd.h>
#include <stdlib.h>
#include <linux/input.h>


#define ts_path	"/dev/input/event1"

//1、打开触摸屏文件
int open_ts(void);

//2.获取触摸屏x、y坐标
void get_xy(int *x, int *y);

//3、关闭触摸屏文件
int close_ts(void);

#endif
