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
    freetype_init( "/usr/share/fonts/ttf/SourceHanSansCN-Regular.otf",0);

    /*if (atoi(argv[2]) == 0)
        show_bmp_image("/linux_prj/image.bmp");
    else if (atoi(argv[2]) == 1)
        show_jpeg_image("/linux_prj/image.jpg");
    else if (atoi(argv[2]) == 2)
        show_png_image("/linux_prj/image.png");*/
        
    while(1)
    {
        lcd_fill(0,1024,0,600,LCD_WHITE);
        //lcd_fill(342,682,0,600,LCD_RED);
        //lcd_fill(683,1024,0,600,LCD_GREEN);
        //show_bmp_image("/linux_prj/image.bmp");

        /* 在 LCD 上显示中文 */
        int y = HEIGHT * 0.25;
        lcd_draw_character(50, 100, L"路漫漫其修远兮，吾将上下而求索", 0x000000);
        lcd_draw_character(50, y+100, L"莫愁前路无知己，天下谁人不识君", 0x9900FF);
        lcd_draw_character(50, 2*y+100, L"君不见黄河之水天上来，奔流到海不复回", 0xFF0099);
        lcd_draw_character(50, 3*y+100, L"君不见高堂明镜悲白发，朝如青丝暮成雪", 0x9932CC);

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
