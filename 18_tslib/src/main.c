#include "ts.h"
#include "led.h"

int main(int argc,char *argv[])
{
    int x,y;

    //打开触摸屏文件
    open_ts();
    open_led_brightness();
    open_led_trigger();

    while(1)
    {
        if(atoi(argv[1]) == 0)
            get_xy(&x,&y);
        else if(atoi(argv[1]) == 1)
            get_xy_tslib(&x,&y);        

        printf("(x,y) = (%d,%d)\r\n",x,y);
        if(x <= 340  && x >=0 && y <= 600 && y >= 0)
            led_switch("on");

        else if(x <= 680  && x >= 341 && y <= 600 && y >= 0)
            led_switch("off");
            
        else if(x <= 1024  && x >= 681 && y <= 600 && y >= 0)
            led_trigger("trigger","heartbeat");
    }
    close_ts();
    return 0;
}
