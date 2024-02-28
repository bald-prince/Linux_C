#ifndef __LCD_H
#define __LCD_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <jpeglib.h>
#include <png.h>
#include <math.h>
#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H

extern unsigned short *screen_base; 

//BMP文件头数据结构
typedef struct {
    unsigned char type[2]; //文件类型
    unsigned int size; //文件大小
    unsigned short reserved1; //保留字段 1
    unsigned short reserved2; //保留字段 2
    unsigned int offset; //到位图数据的偏移量
} __attribute__ ((packed)) bmp_file_header;

//位图信息头数据结构
typedef struct {
    unsigned int size; //位图信息头大小
    int width; //图像宽度
    int height; //图像高度
    unsigned short planes; //位面数
    unsigned short bpp; //像素深度
    unsigned int compression; //压缩方式
    unsigned int image_size; //图像大小
    int x_pels_per_meter; //像素/米
    int y_pels_per_meter; //像素/米
    unsigned int clr_used;
    unsigned int clr_omportant;
} __attribute__ ((packed)) bmp_info_header;

typedef struct bgr888_color {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} __attribute__ ((packed)) bgr888_t;

//lcd屏幕参数宏定义
#define MODE            0                     //1竖屏   0横屏

//竖屏模式
#if MODE
    #define LCD_MAX_Y       600 - 1          //LCD Y 坐标最大值
    #define WIDTH           600                  //LCD X 分辨率
    #define HEIGHT          1024                   //LCD Y 分辨率
    #define USER_WIDTH      600         //竖屏模式下 X 分辨率
    #define USER_HEIGHT     1024        //竖屏模式下 Y 分辨率
    #define LINE_LENGTH     2048        //LCD 一行的长度（字节为单位）
    #define BPP             16          //像素深度
#else
    #define WIDTH           1024                  //LCD X 分辨率
    #define HEIGHT          600                   //LCD Y 分辨率
    #define LINE_LENGTH     2048                  //LCD 一行的长度（字节为单位）
    #define BPP             16                    //像素深度    
#endif

//颜色定义
#define     LCD_BLUE        0x000000FF
#define     LCD_GREEN       0x0000FF00
#define     LCD_RED         0x00FF0000
#define     LCD_ORANGE      0x00FFA500
#define     LCD_TRANSPARENT 0x00000000
#define     LCD_YELLOW      0x00FFFF00
#define     LCD_WHITE       0XFFFFFFFF
//lcd设备路径定义
#define LCD_PATH    "/dev/fb0"
#define CHAR_PATH   "/usr/share/fonts/仿宋_GB2312.ttf"

//1.打开lcd文件
int open_lcd(void);
//2.关闭lcd屏幕
int close_lcd(void);
//3.lcd屏幕操作
//画点函数
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color);
//画线函数
void lcd_draw_line(unsigned int x, unsigned int y, int dir,unsigned int length, unsigned int color);
//RGB888--->RGB565
int argb8888_to_rgb565(unsigned int color);
//画矩形函数
void lcd_draw_rectangle(unsigned int start_x, unsigned int end_x,unsigned int start_y, unsigned int end_y,unsigned int color);
//纯色填充
void lcd_fill(unsigned int start_x, unsigned int end_x,unsigned int start_y, unsigned int end_y,unsigned int color);
//显示bmp
int show_bmp_image(const char *path);
//显示JPEG
int show_jpeg_image(const char *path);
//显示png
int show_png_image(const char *path);
//初始化freetype
int freetype_init(const char *font, int angle);
//显示字符
void lcd_draw_character(int x, int y,const wchar_t *str, unsigned int color,unsigned int size);
void freetype_close(void);

void my_ui(void);
void display_float(int x,int y,float num,unsigned int color,unsigned int size);


#endif
