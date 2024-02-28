#include "display.h"

//显示bmp
int show_bmp_image(const char *path,int x,int y)
{
    bmp_file_header file_h;
    bmp_info_header info_h;
    unsigned short *line_buf = NULL; //行缓冲区
    unsigned long line_bytes; //BMP 图像一行的字节的大小
    unsigned int min_h, min_bytes;
    int fd = -1;
    int j;

    int lcd = open(LCD_PATH,O_RDWR);
    if(-1 == lcd)
    {
        perror("Open lcd error");
        return -1;
    }

    unsigned short *lcd_buf = mmap(	
					    NULL, 					        //默认为NULL,让系统自动分配,首地址
					    WIDTH * HEIGHT * 4,		        //映射空间的大小
					    PROT_READ | PROT_WRITE,	        //允许进行读写操作
					    MAP_SHARED,				        //此内存空间被共享
					    lcd, 				        //lcd的文件描述符
					    0						        //默认为0偏移
				    );

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

    lcd_buf += y * WIDTH + x;

    if (0 < info_h.height) 
    {
        //倒向位图
        if (info_h.height > HEIGHT) 
        {
            min_h = HEIGHT;
            lseek(fd, (info_h.height - HEIGHT) * line_bytes, SEEK_CUR);
            lcd_buf += WIDTH * (HEIGHT - 1); //定位到屏幕左下角位置
        }
        else 
        {
            min_h = info_h.height;
            lcd_buf += WIDTH * (info_h.height - 1); //定位到....不知怎么描述 懂的人自然懂!
        }
        for (j = min_h; j > 0; lcd_buf -= WIDTH, j--) 
        {
            read(fd, line_buf, line_bytes); //读取出图像数据
            memcpy(lcd_buf, line_buf, min_bytes);//刷入 LCD 显存
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
        for (j = 0; j < min_h; j++, lcd_buf += WIDTH) 
        {
            read(fd, line_buf, line_bytes);
            memcpy(lcd_buf, line_buf, min_bytes);
        }
    }
    
    //关闭文件、函数返回
    munmap(lcd_buf,WIDTH * HEIGHT * 4);
    close(fd);
    close(lcd);
    free(line_buf);
    return 0;   
}

int display_image(const char *path,unsigned short *lcd_buf)
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
            lcd_buf += WIDTH * (HEIGHT - 1); //定位到屏幕左下角位置
        }
        else 
        {
            min_h = info_h.height;
            lcd_buf += WIDTH * (info_h.height - 1); //定位到....不知怎么描述 懂的人自然懂!
        }
        for (j = min_h; j > 0; lcd_buf -= WIDTH, j--) 
        {
            read(fd, line_buf, line_bytes); //读取出图像数据
            memcpy(lcd_buf, line_buf, min_bytes);//刷入 LCD 显存
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
        for (j = 0; j < min_h; j++, lcd_buf += WIDTH) 
        {
            read(fd, line_buf, line_bytes);
            memcpy(lcd_buf, line_buf, min_bytes);
        }
    }
    
    //关闭文件、函数返回
    close(fd);
    free(line_buf);
    return 0;   
}