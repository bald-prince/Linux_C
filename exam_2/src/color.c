#include "color.h"

int lcd_fd;
unsigned int *lcd_p;

//1、打开lcd屏幕文件、申请映射内存
int open_lcd_color(void)
{
	lcd_fd = open("/dev/fb0", O_RDWR);
	if (lcd_fd == -1)
	{
		printf("open lcd error!\n");
		return -1;
	}	

	//write(lcd_fd, lcd_buf, 1024*600*4);
	lcd_p =	mmap(	
					NULL, 					//默认为NULL,让系统自动分配,首地址
					1024*600*4,		        //映射空间的大小，也就是开发板现存
					PROT_READ|PROT_WRITE,	//允许进行读写操作
					MAP_SHARED,				//此内存空间被共享
					lcd_fd, 				//lcd的文件描述符
					0						//默认为0偏移
				);

	return 0;
}

//2、显示颜色(写颜色数据到屏幕文件中)
int show_color(int  color)
{
	int lcd_buf[1024*600] = {0};  
	int i, j;
	for(i = 0; i<1024*600; i++)
		lcd_buf[i] = color;         //绿色

	for (j = 0; j < 1024*600; j++)
	{
	 	*(lcd_p+j) = lcd_buf[j];	
	}
	return 0;
}


void draw_circle(int x,int y,int r,unsigned int color)
{
	int temp,i,j;
	for(i = x-r;i < (x+r);i++)
	{
		for(j = y-r;j < (y+r);j++)
		{
			temp = ((i-x)*(i-x))+((j-y)*(j-y));
			if(temp <= (r*r))
				*(lcd_p+j*1024+i) = color;
		}
	}
}

//半圆  lcd结构体  cx,cy圆心坐标 r半径 color圆颜色
void draw_half_circle(int cx,int cy,int r, unsigned int color)
{
	int temp,i,j;
	for(i = cx-r;i < (cx+r);i++)
	{
		for(j = cy-r;j < (cy);j++)
		{
			temp = ((i-cx)*(i-cx))+((j-cy)*(j-cy));
			if(temp <= (r*r))
				*(lcd_p+j*1024+i) = color;
		}
	}
}

//部分清屏  lcd结构体 cx，cy左上角坐标 x，y清屏区域 color清屏颜色
void clear_part_lcd(int cx,int cy,int x,int y,unsigned int color)
{
	int i,j;
	for(i = cx;i < x; i++)
	{
		for(j = cy;j < y; j++)
		{
			*(lcd_p+j*1024+i) = color;
		}
	}
}


void draw_triangle(int x,int y,int h,unsigned int color)
{
	int i,j;
	for(i=0;i<1024;i++)
	{
		for(j=0;j<600;j++)
		{
			if((j-i <= (y-x)) && (j+i <= (x+y)) && (j>=h))
			{
				*(lcd_p+j*1024+i) = color;
			}
		}
	}
}

//画三角形函数  lcd结构  cx,cy圆心坐标 r半径 color圆颜
void draw_triangle_1(int x0,int y0,int x1, int y1, int x2,int y2, unsigned int color)
{
    int i, j;
    double x[3] = {x0, x1, x2}, y[3] = {y0, y1, y2};
    double v0x, v0y, v1x, v1y, v2x, v2y;
    double d00, d01, d11, d20, d21, denom, v, u, w;

    // 利用质心坐标求值
    v0x = x[1] - x[0], v0y = y[1] - y[0];
    v1x = x[2] - x[0], v1y = y[2] - y[0];
    v2x = x[2] - x[1], v2y = y[2] - y[1];
    for (i = 0; i < 1024; i++)
	{
        for (j = 0; j < 600; j++)
        {
            v2x = i - x[0], v2y = j - y[0];
            d00 = v0x*v0x + v0y*v0y;
            d01 = v0x*v1x + v0y*v1y;
            d11 = v1x*v1x + v1y*v1y;
            d20 = v2x*v0x + v2y*v0y;
            d21 = v2x*v1x + v2y*v1y;
            denom = d00*d11 - d01*d01;
            v = (d11*d20 - d01*d21) / denom;
            u = (d00*d21 - d01*d20) / denom;
            w = 1 - u - v;
            printf("v=%d,u=%d,w=%d\n",v,u,w);
            if (0 <= v && v <= 1 && 0 <= u && u <= 1 && 0 <= w && w <= 1)
            {  
				i=1024 - 1 - i;
            	printf("i=%d,j=%d\n",i,j);
                *(lcd_p+j*1024+i) = color;
            }
        }
	}

}
//流动彩虹 lcd结构体 n流动次数
void draw_water_rainbow(int n)
{
	int rainbow_color[8] = {red,orange,gold,forest,cyan,sky,violet,	LCD_WHITE}; 
	int cnt = 0;
	int tmp = 0;
	int z = 0;
				
	for(z = 0;z < n;z++)
	{
		tmp = z;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,200,rainbow_color[tmp]);
		tmp = z+1;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,180,rainbow_color[tmp]);
		tmp = z+2;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,160,rainbow_color[tmp]);
		tmp = z+3;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,140,rainbow_color[tmp]);
		tmp = z+4;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,120,rainbow_color[tmp]);
		tmp = z+5;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,100,rainbow_color[tmp]);
		tmp = z+6;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,80,rainbow_color[tmp]);
		tmp = z+7;if(tmp >= 8) tmp %= 8;
		draw_half_circle(512,300,60,rainbow_color[tmp]);
		while(1)
		{
			cnt++;
			if(cnt == 20000000)
			{
				cnt = 0;
				break;
			}
		}
	}
}


//3、关闭lcd屏幕文件、释放映射内存
int close_lcd_color(void)
{
	//3、关闭lcd屏幕文件
	munmap(lcd_p, 1024*600);
	close(lcd_fd);
	return 0;
}




