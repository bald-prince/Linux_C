#ifndef __LED_H
#define __LED_H
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
#include <string.h>

//触发模式文件路径
#define LED_TRIGGER     "/sys/class/leds/sys-led/trigger"
//常亮模式文件路径
#define LED_BRIGHTNESS  "/sys/class/leds/sys-led/brightness"

//1、打开触摸屏文件
int open_led_trigger(void);
int open_led_brightness(void);

//2.获取触摸屏x、y坐标
void led_switch(char *state);
void led_trigger(char *state,char *heartbeat);


//3、关闭触摸屏文件
int close_led(void);



#endif

