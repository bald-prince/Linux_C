#ifndef _COLOR_H
#define _COLOR_H 

#include <stdio.h>
//open函数
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//read、close、write函数
#include <unistd.h>
//mmap函数
#include <sys/mman.h>

#define     LCD_BLUE        0x000000FF
#define     LCD_GREEN       0x0000FF00
#define     LCD_RED         0x00FF0000
#define     LCD_ORANGE      0x00FFA500
#define     LCD_TRANSPARENT 0x00000000
#define     LCD_YELLOW      0x00FFFF00
#define     LCD_WHITE       0XFFFFFFFF


#define     rose            0x00f05b72   //蔷薇色
#define     cherry          0x00feeeed   //樱花
#define     red             0x00d71345   //红色
#define     orange          0x00f58220   //橘色
#define     gold            0x00cfb53b   //金色
#define     forest          0x00238e23   //森林绿
#define     cyan            0x007fff00   //青色
#define     sky             0x0038b0de   //天蓝色
#define     purple          0x00871f78   //紫色
#define     violet          0x00cc3299   //紫罗兰
#define     black           0x00000000   //




//1、打开lcd屏幕文件、申请映射内存
int open_lcd_color(void);	

//2、显示颜色(写颜色数据到屏幕文件中)
int show_color(int color);

//3、关闭lcd屏幕文件、释放映射内存
int close_lcd_color(void);

void draw_circle(int x,int y,int r,unsigned int color);
void draw_half_circle(int cx,int cy,int r, unsigned int color);
void clear_part_lcd(int cx,int cy,int x,int y, unsigned int color);
void draw_triangle(int x,int y,int h,unsigned int color);
void draw_water_rainbow(int n);
void draw_triangle_1(int x0,int y0,int x1, int y1, int x2,int y2, unsigned int color);

#endif /* color.h */