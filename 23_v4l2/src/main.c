#include "ts.h"
#include "led.h"
#include "lcd.h"
#include "camera.h"
#include <unistd.h>


int main(int argc,char *argv[])
{
    float num = 0.0;
    //打开触摸屏文件
    open_ts();
    open_led_brightness();
    open_led_trigger();
    open_lcd();
    freetype_init(CHAR_PATH,0);
    
    //初始化摄像头
    v4l2_dev_init(CAMERA_PATH);
    //设置格式 
    v4l2_set_format(600,480);
    //初始化帧缓冲：申请、内存映射、入队 
    v4l2_init_buffer();
    //开启视频采集
    v4l2_stream_on();
    
    //my_ui();
    while(1)
    {
        //touch_event_handler();   
        display_float(500,200,num,LCD_BLUE,80);
        usleep(500);
        num += 0.5;
    }
    close_ts();
    close_lcd();
    freetype_close();
    exit(EXIT_SUCCESS);
}
