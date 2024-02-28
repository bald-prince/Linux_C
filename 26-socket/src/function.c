#include "function.h"

int x,y,s_x,s_y,v_x,v_y;
#define NUM_IMAGES 12 // 图片数量

char image_filenames[NUM_IMAGES][10] = {
    "./1.bmp",
    "./2.bmp",
    "./3.bmp",
    "./4.bmp",
    "./5.bmp",
    "./6.bmp",
    "./7.bmp",
    "./8.bmp",
    "./9.bmp",
    "./10.bmp",
    "./11.bmp",
    "./12.bmp"
};

void function(void)
{
    open_ts();
    while(1)
    {
        show_bmp_image("./gongneng.bmp",0,0);
        get_xy(&x,&y);
        if(x > 80 && x < 280 && y > 90 && y < 260)              //相册
        {
            show_bmp_image("./zishou.bmp",0,0);
            while(1)
            {
                get_xy(&s_x,&s_y);
                if(s_x > 830 && s_x < 970 && s_y > 220 && s_y < 360)
                {
                    printf("自动播放\r\n");
                    break;
                }
                else if(s_x > 70 && s_x < 230 && s_y > 220 && s_y < 360)
                {
                    printf("手动播放\r\n");
                    run_album();
                    break;
                }
                else if(s_x > 420 && s_x < 600 && s_y > 100 && s_y < 360)
                {
                    break;
                }  
            }
        }
        if(x > 390 && x < 600 && y > 90 && y < 260)            //语音识别
        {

        }
        if(x > 720 && x < 880 && y > 90 && y < 260)            //监控系统
        {
            show_bmp_image("./camera.bmp",0,0);
            my_camera();
            break;
        }

        if(x > 75 && x < 340 && y > 340 && y < 480)            //视频
        {
            //system("mplayer ./first.avi& -fs");
            system("mplayer -slave -quiet -input file=/pipe -geometry 0:0 -zoom -fs ./first.avi&");
            get_xy(&v_x,&v_y);
            printf("(x,y) = (%d,%d)\r\n",v_x,v_y);
			if(v_x > 0 && v_y > 0 &&v_x < 1024 && v_y < 600)
			{
				system("killall -9 mplayer");
				show_bmp_image("./gongneng.bmp", 0, 0);
				break;
			}			
        }

        if(x > 390 && x < 600 && y > 340 && y < 480)            //音乐
        {
            //system("mplayer ./0.mp3& ");
            system("mplayer -slave -quiet -input file=/pipe -geometry 0:0 -zoom -x 800 -y 480 ./0.mp3&");
            get_xy(&v_x,&v_y);
            printf("(x,y) = (%d,%d)\r\n",v_x,v_y);
			if(v_x > 0 && v_y > 0 &&v_x < 1024 && v_y < 600)
			{
				system("killall -9 mplayer");
				show_bmp_image("./gongneng.bmp", 0, 0);
				break;
			}		
        }



    }
}

void run_album(void)
{
    // 打开 LCD 设备驱动
    int lcd = open(LCD_PATH, O_RDWR);
    if (lcd < 0) 
    {
        perror("open lcd");
        return;
    }

    // 申请虚拟共享内存
    unsigned short *lcd_buf = mmap(
            NULL, 
            WIDTH * HEIGHT * 4, 
            PROT_READ | PROT_WRITE, 
            MAP_SHARED, 
            lcd, 
            0
        );
        
    if (lcd_buf == MAP_FAILED) 
    {
        perror("mmap lcd");
        close(lcd);
        return;
    }

    int current_image = 0;
    int x = 0, y = 0;
    int touch_count = 0;  // 点击屏幕中心的计数器

    while(1)
    {
        display_image(image_filenames[current_image], lcd_buf);
        // 等待触摸屏操作
        get_xy(&x, &y);

        // 判断滑动和点击操作
        if (x < WIDTH / 3) 
        {
            current_image = (current_image - 1 + NUM_IMAGES) % NUM_IMAGES; // 向左滑动
        } 
        else if (x > WIDTH * 2 / 3) 
        {
            current_image = (current_image + 1) % NUM_IMAGES; // 向右滑动
        } 
        else if (x > WIDTH / 3 && x < WIDTH * 2 / 3 && y > HEIGHT / 3 && y < HEIGHT * 2 / 3) 
        {
            // 点击屏幕中心
            touch_count++;
            if (touch_count == 2) 
            {
                //退出循环界面
                break;
            }
        }

        printf("Current Image: %s\n", image_filenames[current_image]);
    }

        // 解除映射并关闭文件
    munmap(lcd_buf, WIDTH * HEIGHT * 4);
    close(lcd);
}