#include "lcd.h"

int lcd_fd;
static unsigned short *screen_base = NULL;      //映射后的显存基地址

//1.打开lcd屏幕
int open_lcd(void)
{
    lcd_fd = open(lcd_path,O_RDWR);
    if(-1 == lcd_fd)
    {
        perror("Open lcd error");
        return -1;
    }

    screen_base = mmap(	
					    NULL, 					        //默认为NULL,让系统自动分配,首地址
					    WIDTH * HEIGHT * 4,		        //映射空间的大小
					    PROT_READ | PROT_WRITE,	        //允许进行读写操作
					    MAP_SHARED,				        //此内存空间被共享
					    lcd_fd, 				        //lcd的文件描述符
					    0						        //默认为0偏移
				    );

    memset(screen_base, 0xFF, WIDTH * HEIGHT * 4);

    return 0;
}

//2.关闭lcd屏幕
int close_lcd(void)
{
    close(lcd_fd);
    munmap(screen_base, WIDTH * HEIGHT * 4); //取消映射
    return 0;
}

//3.lcd屏幕操作
//画点函数
void lcd_draw_point(unsigned int x, unsigned int y, unsigned int color)
{
    unsigned short rgb565_color = argb8888_to_rgb565(color);//得到 RGB565 颜色值

    /* 对传入参数的校验 */
    if (x >= WIDTH)
        x = WIDTH - 1;
    if (y >= HEIGHT)
        y = HEIGHT - 1;

    /* 填充颜色 */
    screen_base[y * WIDTH + x] = rgb565_color;
}

//画线函数
void lcd_draw_line(unsigned int x, unsigned int y, int dir,unsigned int length, unsigned int color)
{
    unsigned short rgb565_color = argb8888_to_rgb565(color);//得到 RGB565 颜色值
    unsigned int end;
    unsigned long temp;
    /* 对传入参数的校验 */
    if (x >= WIDTH)
        x = WIDTH - 1;
    if (y >= HEIGHT)
        y = HEIGHT - 1;
    /* 填充颜色 */
    temp = y * WIDTH + x;//定位到起点
    if (dir) 
    { 
        //水平线
        end = x + length - 1;
        if (end >= WIDTH)
            end = WIDTH - 1;
        for ( ; x <= end; x++, temp++)
            screen_base[temp] = rgb565_color;
    }
    else 
    { 
        //垂直线
        end = y + length - 1;
        if (end >= HEIGHT)
            end = HEIGHT - 1;
        for ( ; y <= end; y++, temp += WIDTH)
            screen_base[temp] = rgb565_color;
    }
}

//RGB888--->RGB565
int argb8888_to_rgb565(unsigned int color)
{
    unsigned int temp = color;
    return (unsigned short)(
                                ((temp & 0xF80000UL) >> 8) |
                                ((temp & 0xFC00UL) >> 5) |
                                ((temp & 0xF8UL) >> 3)
                            );
}

//画矩形函数
void lcd_draw_rectangle(unsigned int start_x, unsigned int end_x,unsigned int start_y, unsigned int end_y,unsigned int color)
{
    int x_len = end_x - start_x + 1;
    int y_len = end_y - start_y - 1;
    lcd_draw_line(start_x, start_y, 1, x_len, color);//上边
    lcd_draw_line(start_x, end_y, 1, x_len, color); //下边
    lcd_draw_line(start_x, start_y + 1, 0, y_len, color);//左边
    lcd_draw_line(end_x, start_y + 1, 0, y_len, color);//右边
}

//纯色填充
void lcd_fill(unsigned int start_x, unsigned int end_x,unsigned int start_y, unsigned int end_y,unsigned int color)
{
    unsigned short rgb565_color = argb8888_to_rgb565(color);//得到 RGB565 颜色值
    unsigned long temp;
    unsigned int x;

    /* 对传入参数的校验 */
    if (end_x >= WIDTH)
        end_x = WIDTH - 1;
    if (end_y >= HEIGHT)
        end_y = HEIGHT - 1;

    /* 填充颜色 */
    temp = start_y * WIDTH; //定位到起点行首
    for ( ; start_y <= end_y; start_y++, temp += WIDTH) 
    {
        for (x = start_x; x <= end_x; x++)
            screen_base[temp + x] = rgb565_color;
    }
}

//显示bmp
int show_bmp_image(const char *path)
{
    bmp_file_header file_h;
    bmp_info_header info_h;
    unsigned short *line_buf = NULL; //行缓冲区
    unsigned long line_bytes; //BMP 图像一行的字节的大小
    unsigned int min_h, min_bytes;
    int fd = -1;
    int j;
    /* 打开文件 */
    if (0 > (fd = open(path, O_RDONLY))) 
    {
        perror("open error");
        return -1;
    }
    /* 读取 BMP 文件头 */
    if (sizeof(bmp_file_header) != read(fd, &file_h, sizeof(bmp_file_header))) 
    {
        perror("read error");
        close(fd);
        return -1;
    }
    if (0 != memcmp(file_h.type, "BM", 2)) 
    {
        fprintf(stderr, "it's not a BMP file\n");
        close(fd);
        return -1;
    }

    /* 读取位图信息头 */
    if (sizeof(bmp_info_header) != read(fd, &info_h, sizeof(bmp_info_header))) 
    {
        perror("read error");
        close(fd);
        return -1;
    }

    /* 将文件读写位置移动到图像数据开始处 */
    if (-1 == lseek(fd, file_h.offset, SEEK_SET)) 
    {
        perror("lseek error");
        close(fd);
        return -1;
    }

    /* 申请一个 buf、暂存 bmp 图像的一行数据 */
    line_bytes = info_h.width * info_h.bpp / 8;
    line_buf = malloc(line_bytes);
    if (NULL == line_buf) 
    {
        fprintf(stderr, "malloc error\n");
        close(fd);
        return -1;
    }
    if (LINE_LENGTH > line_bytes)
        min_bytes = line_bytes;
    else
        min_bytes = LINE_LENGTH;

    if (0 < info_h.height) 
    {
        //倒向位图
        if (info_h.height > HEIGHT) 
        {
            min_h = HEIGHT;
            lseek(fd, (info_h.height - HEIGHT) * line_bytes, SEEK_CUR);
            screen_base += WIDTH * (HEIGHT - 1); //定位到屏幕左下角位置
        }
        else 
        {
            min_h = info_h.height;
            screen_base += WIDTH * (info_h.height - 1); //定位到....不知怎么描述 懂的人自然懂!
        }
        for (j = min_h; j > 0; screen_base -= WIDTH, j--) 
        {
            read(fd, line_buf, line_bytes); //读取出图像数据
            memcpy(screen_base, line_buf, min_bytes);//刷入 LCD 显存
        }
    }
    else 
    { 
        //正向位图
        int temp = 0 - info_h.height; //负数转成正数
        if (temp > HEIGHT)
            min_h = HEIGHT;
        else
        min_h = temp;
        for (j = 0; j < min_h; j++, screen_base += WIDTH) 
        {
            read(fd, line_buf, line_bytes);
            memcpy(screen_base, line_buf, min_bytes);
        }
    }
    //关闭文件、函数返回
    close(fd);
    free(line_buf);
    return 0;   
}



