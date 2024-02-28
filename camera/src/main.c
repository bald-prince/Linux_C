#include "ts.h"
#include "led.h"
#include "lcd.h"
#include "camera.h"
#include "display.h"
#include <unistd.h>
#include "function.h"

int main(int argc,char *argv[])
{
    v4l2_dev_init(CAMERA_PATH);     //初始化摄像头
    v4l2_set_format(720,576);       //设置格式 
    v4l2_init_buffer();             //初始化帧缓冲：申请、内存映射、入队 
    open_ts();                      //打开触摸屏文件
    open_led_brightness();
    open_led_trigger();
    open_lcd();
    freetype_init(CHAR_PATH,0); 
    
    while(1)
    {
        function();
    }
    close_ts();
    close_lcd();
    freetype_close();
    exit(EXIT_SUCCESS);
}
