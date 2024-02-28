#include "ts.h"
#include "led.h"
#include "lcd.h"

int main(int argc,char *argv[])
{
    int x,y;

    //打开触摸屏文件
    open_ts();
    open_led_brightness();
    open_led_trigger();
    open_lcd();
    freetype_init(CHAR_PATH,0);

    //在 LCD 上显示中文
    lcd_fill(0,1024,0,600,LCD_WHITE);
    lcd_draw_line(340,0,0,600,LCD_RED);
    lcd_draw_line(680,0,0,600,LCD_RED);
    lcd_draw_character(100, 300, L"开灯", LCD_RED,80);
    lcd_draw_character(440, 300, L"关灯", LCD_RED,80);
    lcd_draw_character(780, 300, L"闪烁", LCD_RED,80);

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
    close_lcd();
    freetype_close();
    exit(EXIT_SUCCESS);
}
