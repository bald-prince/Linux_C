#ifndef __CAMERA_H
#define __CAMERA_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <linux/fb.h>
#include "lcd.h"
#include <jpeglib.h>
#include <png.h>
#include <math.h>
#include <wchar.h>
#include <ft2build.h>
#include <signal.h>
#include <pthread.h>


/*
format<0x50424752>, description<RGB565_LE>
size<640*480> <15fps><30fps>
size<320*240> <15fps><30fps>
size<720*480> <15fps><30fps>
size<720*576> <15fps><30fps>
size<1280*720> <15fps><30fps>
size<1920*1080> <15fps>
size<2592*1944> <15fps>
size<176*144> <15fps><30fps>
size<1024*768> <15fps><30fps>
size<480*272> <15fps><30fps>
size<800*480> <15fps><30fps>
size<1024*600> <15fps><30fps>
size<1280*800> <15fps><30fps>

format<0x4745504a>, description<JPEG>
size<640*480> <15fps><30fps>
size<320*240> <15fps><30fps>
size<720*480> <15fps><30fps>
size<720*576> <15fps><30fps>
size<1280*720> <15fps><30fps>
size<1920*1080> <15fps>
size<2592*1944> <15fps>
size<176*144> <15fps><30fps>
size<1024*768> <15fps><30fps>
size<480*272> <15fps><30fps>
size<800*480> <15fps><30fps>
size<1024*600> <15fps><30fps>
size<1280*800> <15fps><30fps>

format<0x56595559>, description<YUYV-16>
size<640*480> <15fps><30fps>
size<320*240> <15fps><30fps>
size<720*480> <15fps><30fps>
size<720*576> <15fps><30fps>
size<1280*720> <15fps><30fps>
size<1920*1080> <15fps>
size<2592*1944> <15fps>
size<176*144> <15fps><30fps>
size<1024*768> <15fps><30fps>
size<480*272> <15fps><30fps>
size<800*480> <15fps><30fps>
size<1024*600> <15fps><30fps>
size<1280*800> <15fps><30fps>
*/

extern volatile int v4l2_state;
pthread_t v4l2_thread; // 全局线程变量
#define FRAMEBUFFER_COUNT 3 //帧缓冲数量
#define CAMERA_PATH     "/dev/video1"


//摄像头像素格式及其描述信息
typedef struct camera_format {
    unsigned char description[32]; //字符串描述信息
    unsigned int pixelformat; //像素格式
} cam_fmt;

//描述一个帧缓冲的信息
typedef struct cam_buf_info {
    unsigned short *start; //帧缓冲起始地址
    unsigned long length; //帧缓冲长度
} cam_buf_info;

//1.camera初始化
int v4l2_dev_init(const char *device);
int v4l2_set_format(int width,int height);
int v4l2_init_buffer(void);
int v4l2_stream_on(void);
void v4l2_read_data(void);

void *show_video(void *arg);

#endif


