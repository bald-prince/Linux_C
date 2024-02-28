#include "lcd.h"

int lcd_fd;
static unsigned short *screen_base = NULL;      //映射后的显存基地址
static FT_Library library;
static FT_Face face;

//1.打开lcd屏幕
int open_lcd(void)
{
    lcd_fd = open(LCD_PATH,O_RDWR);
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
    #if MODE
        screen_base[(LCD_MAX_Y-x) * WIDTH + y] = rgb565_color;
    #else
        screen_base[y * WIDTH + x] = rgb565_color;
    #endif
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

//显示JPEG
int show_jpeg_image(const char *path)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *jpeg_file = NULL;
    bgr888_t *jpeg_line_buf = NULL; //行缓冲区:用于存储从 jpeg 文件中解压出来的一行图像数据
    unsigned short *fb_line_buf = NULL; //行缓冲区:用于存储写入到 LCD 显存的一行数据
    unsigned int min_h, min_w;
    unsigned int valid_bytes;
    int i;

    //绑定默认错误处理函数
    cinfo.err = jpeg_std_error(&jerr);

    //打开.jpeg/.jpg 图像文件
    jpeg_file = fopen(path, "r"); //只读方式打开
    if (NULL == jpeg_file) 
    {
        perror("fopen error");
        return -1;
    }

    //创建 JPEG 解码对象
    jpeg_create_decompress(&cinfo);
    //指定图像文件
    jpeg_stdio_src(&cinfo, jpeg_file);
    //读取图像信息
    jpeg_read_header(&cinfo, TRUE);

    //printf("jpeg 图像大小: %d*%d\n", cinfo.image_width, cinfo.image_height);

    //设置解码参数
    cinfo.out_color_space = JCS_RGB;//默认就是 JCS_RGB
    //cinfo.scale_num = 1;
    //cinfo.scale_denom = 2;

    //开始解码图像
    jpeg_start_decompress(&cinfo);

    //为缓冲区分配内存空间
    jpeg_line_buf = malloc(cinfo.output_components * cinfo.output_width);
    fb_line_buf = malloc(LINE_LENGTH);
    
    //判断图像和 LCD 屏那个的分辨率更低
    if (cinfo.output_width > WIDTH)
        min_w = WIDTH;
    else
        min_w = cinfo.output_width;

    if (cinfo.output_height > HEIGHT)
        min_h = HEIGHT;
    else
        min_h = cinfo.output_height;

    //读取数据
    valid_bytes = min_w * BPP / 8;      //一行的有效字节数 表示真正写入到 LCD 显存的一行数据的大小

    while (cinfo.output_scanline < min_h) 
    {
        jpeg_read_scanlines(&cinfo, (unsigned char **)&jpeg_line_buf, 1);//每次读取一行数据
        //将读取到的 BGR888 数据转为 RGB565
        for (i = 0; i < min_w; i++)
            fb_line_buf[i] = ((jpeg_line_buf[i].red & 0xF8) << 8) |
                                ((jpeg_line_buf[i].green & 0xFC) << 3) |
                                ((jpeg_line_buf[i].blue & 0xF8) >> 3);
        memcpy(screen_base, fb_line_buf, valid_bytes);
        screen_base += WIDTH;       //+width 定位到 LCD 下一行显存地址的起点
    }

    //解码完成
    jpeg_finish_decompress(&cinfo); //完成解码
    jpeg_destroy_decompress(&cinfo);//销毁 JPEG 解码对象、释放资源

    //关闭文件、释放内存
    fclose(jpeg_file);
    free(fb_line_buf);
    free(jpeg_line_buf);
    return 0;
}

//显示png
int show_png_image(const char *path)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;          //描述了png图像的信息
    FILE *png_file = NULL;
    unsigned short *fb_line_buf = NULL; //行缓冲区:用于存储写入到 LCD 显存的一行数据
    unsigned int min_h, min_w;
    unsigned int valid_bytes;
    unsigned int image_h, image_w;
    png_bytepp row_pointers = NULL;
    int i, j, k;

    /* 打开 png 文件 */
    png_file = fopen(path, "r"); //以只读方式打开
    if (NULL == png_file) 
    {
        perror("fopen error");
        return -1;
    }
    /* 分配和初始化 png_ptr、 info_ptr */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);      //libpng版本号信息：PNG_LIBPNG_VER_STRING
    if (!png_ptr) 
    {
        fclose(png_file);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) 
    {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(png_file);
        return -1;
    }

    /* 设置错误返回点 */
    if (setjmp(png_jmpbuf(png_ptr))) 
    {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(png_file);
        return -1;
    }

    /* 指定数据源 */
    png_init_io(png_ptr, png_file);

    /* 读取png文件 */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_ALPHA, NULL);           //PNG_TRANSFORM_STRIP_ALPHA：删除alpha通道
    image_h = png_get_image_height(png_ptr, info_ptr);
    image_w = png_get_image_width(png_ptr, info_ptr);
    
    /* 判断是不是 RGB888 */
    if ((8 != png_get_bit_depth(png_ptr, info_ptr)) && (PNG_COLOR_TYPE_RGB != png_get_color_type(png_ptr, info_ptr))) 
    {
        printf("Error: Not 8bit depth or not RGB color");
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(png_file);
        return -1;
    }
    /* 判断图像和 LCD 屏那个的分辨率更低 */
    if (image_w > WIDTH)
        min_w = WIDTH;
    else
        min_w = image_w;

    if (image_h > HEIGHT)
        min_h = HEIGHT;
    else
        min_h = image_h;

    valid_bytes = min_w * BPP / 8;

    /* 读取解码后的数据 */
    fb_line_buf = malloc(valid_bytes);
    row_pointers = png_get_rows(png_ptr, info_ptr);//获取数据
    unsigned int temp = min_w * 3; //RGB888 一个像素 3 个 bit 位
    for(i = 0; i < min_h; i++) 
    {
        // RGB888 转为 RGB565
        for(j = k = 0; j < temp; j += 3, k++)
            fb_line_buf[k] =    ((row_pointers[i][j] & 0xF8) << 8) |
                                ((row_pointers[i][j+1] & 0xFC) << 3) |
                                ((row_pointers[i][j+2] & 0xF8) >> 3);

        memcpy(screen_base, fb_line_buf, valid_bytes);//将一行数据刷入显存
        screen_base += WIDTH; //定位到显存下一行
    }
    /* 结束、销毁/释放内存 */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    free(fb_line_buf);
    fclose(png_file);
    return 0;
}

//初始化freetype
int freetype_init(const char *font, int angle)
{
    FT_Error error;
    FT_Vector pen;
    FT_Matrix matrix;

    float rad; //旋转角度
    /* FreeType 初始化 */
    FT_Init_FreeType(&library);

    /* 加载 face 对象 */
    error = FT_New_Face(library, font, 0, &face);
    if(error) 
    {
        fprintf(stderr, "FT_New_Face error: %d\n", error);
        exit(EXIT_FAILURE);
    }

    /* 原点坐标 */
    pen.x = 0 * 64;
    pen.y = 0 * 64; //原点设置为(0, 0)

    /* 2x2 矩阵初始化 */
    rad = (1.0 * angle / 180) * M_PI; //（角度转换为弧度） M_PI 是圆周率

    #if 0 //非水平方向
        matrix.xx = (FT_Fixed)( cos(rad) * 0x10000L);
        matrix.xy = (FT_Fixed)(-sin(rad) * 0x10000L);
        matrix.yx = (FT_Fixed)( sin(rad) * 0x10000L);
        matrix.yy = (FT_Fixed)( cos(rad) * 0x10000L);
    #endif

    #if 1 //斜体 水平方向显示的
        matrix.xx = (FT_Fixed)( cos(rad) * 0x10000L);
        matrix.xy = (FT_Fixed)( sin(rad) * 0x10000L);
        matrix.yx = (FT_Fixed)( 0 * 0x10000L);
        matrix.yy = (FT_Fixed)( 1 * 0x10000L);
    #endif
    /* 设置 */
    FT_Set_Transform(face, &matrix, &pen);
    FT_Set_Pixel_Sizes(face, 50, 0); //设置字体大小
    return 0;
}

void freetype_close(void)
{
    FT_Done_Face(face);
    FT_Done_FreeType(library); 
}

//显示字符
void lcd_draw_character(int x, int y,const wchar_t *str, unsigned int color,unsigned int size)
{
    unsigned short rgb565_color = argb8888_to_rgb565(color);//得到 RGB565 颜色值
    FT_GlyphSlot slot = face->glyph;
    size_t len = wcslen(str); //计算字符的个数
    long int temp;
    int n;
    int i, j, p, q;
    int max_x, max_y, start_y, start_x;
    FT_Set_Pixel_Sizes(face, size, 0); //设置字体大小
    // 循环加载各个字符
    for (n = 0; n < len; n++) 
    {
        // 加载字形、转换得到位图数据
        if (FT_Load_Char(face, str[n], FT_LOAD_RENDER))
        continue;
        start_y = y - slot->bitmap_top; //计算字形轮廓上边 y 坐标起点位置 注意是减去 bitmap_top
        if (0 > start_y) 
        {
            //如果为负数 如何处理？？
            q = -start_y;
            temp = 0;
            j = 0;
        }
        else 
        { 
            // 正数又该如何处理??
            q = 0;
            temp = WIDTH * start_y;
            j = start_y;
        }
        max_y = start_y + slot->bitmap.rows;//计算字形轮廓下边 y 坐标结束位置
        if (max_y > (int)HEIGHT)
            max_y = HEIGHT;
        for (; j < max_y; j++, q++, temp += WIDTH) 
        {
            start_x = x + slot->bitmap_left; //起点位置要加上左边空余部分长度
            if (0 > start_x) 
            {
                p = -start_x;
                i = 0;
            }
            else 
            {
                p = 0;
                i = start_x;
            }
            max_x = start_x + slot->bitmap.width;
            if (max_x > (int)WIDTH)
            max_x = WIDTH;
            for (; i < max_x; i++, p++) 
            {
                // 如果数据不为 0，则表示需要填充颜色
                if (slot->bitmap.buffer[q * slot->bitmap.width + p])
                screen_base[temp + i] = rgb565_color;
            }
        }
        //调整到下一个字形的原点
        x += slot->advance.x / 64; //26.6 固定浮点格式
        y -= slot->advance.y / 64;
    }
}
