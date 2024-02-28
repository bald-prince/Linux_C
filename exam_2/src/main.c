#include "color.h"
#include "ts.h"

/*****************************************/
//时间：2021.6.13
//功能：实现触摸切换屏幕颜色和显示图形位置、颜色
//使用步骤：见readme.txt
/*****************************************/

int main(int argc, char const *argv[])
{
    int x,y;
    open_ts();
    open_lcd_color();
    show_color(LCD_WHITE);
    while (1)
    {
        get_xy(&x,&y);
        if(x < 512 && x > 0 && y < 300 && y > 0)
        {
            printf("x = %d  y = %d\r\n",x,y);
            show_color(LCD_BLUE);
            draw_circle(100,100,50,LCD_YELLOW);
            draw_triangle(512,300,50,LCD_RED);
        }
        else if (x > 512 && x < 1024 && y < 300 && y > 0)
        {
            printf("x = %d  y = %d\r\n",x,y);
            show_color(cherry);
            draw_water_rainbow(50);
        }
        else if (x < 512 && x > 0 && y < 600 && y > 300)
        {
            printf("x = %d  y = %d\r\n",x,y);
            show_color(cyan);
            draw_circle(400,400,80,LCD_ORANGE);
            draw_triangle(500,100,50,purple);
        }
        else
        {
            printf("x = %d  y = %d\r\n",x,y);
            show_color(LCD_WHITE);
        } 
    }
    close_lcd_color();
    close_ts();
    return 0;
}
            
            