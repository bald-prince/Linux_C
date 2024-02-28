#include "camera.h"

int v4l2_fd; //摄像头设备文件描述符

static cam_buf_info buf_infos[FRAMEBUFFER_COUNT];
static int frm_width, frm_height; //视频帧宽度和高度
pthread_t v4l2_thread = 0;
//1.camera初始化
int v4l2_dev_init(const char *device)
{
    struct v4l2_capability cap = {0};
    /* 打开摄像头 */
    v4l2_fd = open(device, O_RDWR);
    if (0 > v4l2_fd) 
    {
        fprintf(stderr, "open error: %s: %s\n", device, strerror(errno));
        return -1;
    }
    /* 查询设备功能 */
    ioctl(v4l2_fd, VIDIOC_QUERYCAP, &cap);
    /* 判断是否是视频采集设备 */
    if (!(V4L2_CAP_VIDEO_CAPTURE & cap.capabilities)) 
    {
        fprintf(stderr, "Error: %s: No capture video device!\n", device);
        close(v4l2_fd);
        return -1;
    }
    return 0;
}

//2.设置格式
int v4l2_set_format(int width,int height)
{
    struct v4l2_format fmt = {0};
    struct v4l2_streamparm streamparm = {0};
    /* 设置帧格式 */
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;             //type 类型
    fmt.fmt.pix.width = width;                          //视频帧宽度
    fmt.fmt.pix.height = height;                        //视频帧高度
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;      //像素格式
    if (0 > ioctl(v4l2_fd, VIDIOC_S_FMT, &fmt)) 
    {
        fprintf(stderr, "ioctl error: VIDIOC_S_FMT: %s\n", strerror(errno));
        return -1;
    }
    /* 判断是否已经设置为我们要求的 RGB565 像素格式
    如果没有设置成功表示该设备不支持 RGB565 像素格式 */
    if (V4L2_PIX_FMT_RGB565 != fmt.fmt.pix.pixelformat)
    {
        fprintf(stderr, "Error: the device does not support RGB565 format!\n");
        return -1;
    }
    frm_width = fmt.fmt.pix.width; //获取实际的帧宽度
    frm_height = fmt.fmt.pix.height;//获取实际的帧高度
    printf("视频帧大小<%d * %d>\n", frm_width, frm_height);
    //获取 streamparm
    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ioctl(v4l2_fd, VIDIOC_G_PARM, &streamparm);
    //判断是否支持帧率设置
    if (V4L2_CAP_TIMEPERFRAME & streamparm.parm.capture.capability) 
    {
        streamparm.parm.capture.timeperframe.numerator = 1;
        streamparm.parm.capture.timeperframe.denominator = 30;      //30fps
        if (0 > ioctl(v4l2_fd, VIDIOC_S_PARM, &streamparm))
        {
            fprintf(stderr, "ioctl error: VIDIOC_S_PARM: %s\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

int v4l2_init_buffer(void)
{
    struct v4l2_requestbuffers reqbuf = {0};
    struct v4l2_buffer buf = {0};
    /* 申请帧缓冲 */
    reqbuf.count = FRAMEBUFFER_COUNT; //帧缓冲的数量
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    if (0 > ioctl(v4l2_fd, VIDIOC_REQBUFS, &reqbuf)) 
    {
        fprintf(stderr, "ioctl error: VIDIOC_REQBUFS: %s\n", strerror(errno));
        return -1;
    }
    //建立内存映射
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    for (buf.index = 0; buf.index < FRAMEBUFFER_COUNT; buf.index++) 
    {
        ioctl(v4l2_fd, VIDIOC_QUERYBUF, &buf);
        buf_infos[buf.index].length = buf.length;
        buf_infos[buf.index].start = mmap(NULL, buf.length,
        PROT_READ | PROT_WRITE, MAP_SHARED,
        v4l2_fd, buf.m.offset);
        if (MAP_FAILED == buf_infos[buf.index].start) 
        {
            perror("mmap error");
            return -1;
        }
    }
    /* 入队 */
    for (buf.index = 0; buf.index < FRAMEBUFFER_COUNT; buf.index++) 
    {
        if (0 > ioctl(v4l2_fd, VIDIOC_QBUF, &buf)) 
        {
            perror("VIDIOC_QBUF");
            return -1;
        }
    }
    return 0;
}

int v4l2_stream_on(void)
{
    /* 打开摄像头、摄像头开始采集数据 */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 > ioctl(v4l2_fd, VIDIOC_STREAMON, &type)) 
    {
        perror("VIDIOC_STREAMON");
        return -1;
    }
    return 0;
}

int v4l2_stream_off(void)
{
    /* 打开摄像头、摄像头开始采集数据 */
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (0 > ioctl(v4l2_fd, VIDIOC_STREAMOFF, &type)) 
    {
        perror("VIDIOC_STREAMOFF");
        return -1;
    }
    return 0;
}

void v4l2_read_data(void)
{
    struct v4l2_buffer buf = {0};
    unsigned short *base;
    unsigned short *start;
    int min_w, min_h;
    int j;

    int lcd_fd = open(LCD_PATH,O_RDWR);
    if(-1 == lcd_fd)
        perror("Open lcd error");

    unsigned short *lcd_buf = mmap(	
					    NULL, 					        //默认为NULL,让系统自动分配,首地址
					    WIDTH * HEIGHT * 4,		        //映射空间的大小
					    PROT_READ | PROT_WRITE,	        //允许进行读写操作
					    MAP_SHARED,				        //此内存空间被共享
					    lcd_fd, 				        //lcd的文件描述符
					    0						        //默认为0偏移
				    );

    if (WIDTH > frm_width)
        min_w = frm_width;
    else
        min_w = WIDTH;

    if (HEIGHT > frm_height)
        min_h = frm_height;
    else
        min_h = HEIGHT;

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
    while(1)
    {
        for(buf.index = 0; buf.index < FRAMEBUFFER_COUNT; buf.index++) 
        {
            ioctl(v4l2_fd, VIDIOC_DQBUF, &buf); //出队
            for (j = 0, base = lcd_buf, start = buf_infos[buf.index].start;j < min_h; j++) 
            {
                memcpy(base, start, min_w * 2); //RGB565 一个像素占 2 个字节
                base += WIDTH; //LCD 显示指向下一行
                start += frm_width;//指向下一行数据
            }
            // 数据处理完之后、再入队、往复
            ioctl(v4l2_fd, VIDIOC_QBUF, &buf);
        }
    }
    munmap(lcd_buf, WIDTH * HEIGHT * 4); //取消映射
}

volatile int camera_state = 1;
volatile int capture_state = 0;
int jpg_num = 0;
void *show_video(void *arg)
{
    pthread_detach(pthread_self());     //自行分离
    struct v4l2_buffer buf = {0};
    unsigned short *base;
    unsigned short *start;
    int min_w, min_h;
    int j;
    char jpg_path[100] = {0};

    //打开lcd文件
    int lcd = open(LCD_PATH,O_RDWR);
    if(-1 == lcd)
        perror("Open lcd error");
    //映射lcd缓冲区
    unsigned short *lcd_buf = mmap(	
					    NULL, 					        //默认为NULL,让系统自动分配,首地址
					    WIDTH * HEIGHT * 4,		        //映射空间的大小
					    PROT_READ | PROT_WRITE,	        //允许进行读写操作
					    MAP_SHARED,				        //此内存空间被共享
					    lcd, 				            //lcd的文件描述符
					    0						        //默认为0偏移
				    );

    if (WIDTH > frm_width)
        min_w = frm_width;
    else
        min_w = WIDTH;

    if (HEIGHT > frm_height)
        min_h = frm_height;
    else
        min_h = HEIGHT;

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // lcd_buf += 10 * WIDTH + 50;
    while(camera_state)
    {
        for(buf.index = 0; buf.index < FRAMEBUFFER_COUNT; buf.index++) 
        {
            ioctl(v4l2_fd, VIDIOC_DQBUF, &buf); //出队
            for (j = 0, base = lcd_buf, start = buf_infos[buf.index].start;j < min_h; j++) 
            {
                memcpy(base, start, min_w * 2); //RGB565 一个像素占 2 个字节
                base += WIDTH; //LCD 显示指向下一行
                start += frm_width;//指向下一行数据
            }
            if(capture_state)
            {
                sprintf(jpg_path,"/home/root/share_file/camera_prj/zhuapai_%d.jpg",jpg_num);
                save_as_jpg(lcd_buf, 0,0, frm_width, frm_height, 95, jpg_path);  
            }
            capture_state = 0; 
            //数据处理完之后、再入队、往复
            ioctl(v4l2_fd, VIDIOC_QBUF, &buf);
        }
    }
    munmap(lcd_buf, WIDTH * HEIGHT * 4); //取消映射
    pthread_exit(NULL);
    //pthread_cancel(v4l2_thread);            
    //pthread_join(v4l2_thread, NULL);  
}

/****************************************************************
 *   保存指定区域图像到jpg文件
 *   @param buffer: 图像数据指针
 *   @param x: 起始x坐标
 *   @param y: 起始y坐标
 *   @param region_width: 图像宽度
 *   @param region_height: 图像高度
 *   @param quality：图像压缩质量
 *   @param filename：文件保存路径
****************************************************************/
int save_as_jpg(unsigned short *buffer, int x, int y, int region_width, int region_height,int quality, const char *filename)
{
    //打开文件
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        perror("Error opening file for writing");
        return -1;
    }
    //jpeg压缩结构体
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    //设置错误处理器并创建 JPEG 压缩对象
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    //设置错误处理器并创建 JPEG 压缩对象
    jpeg_stdio_dest(&cinfo, file);
    //设置图像属性
    cinfo.image_width = region_width;       //宽度
    cinfo.image_height = region_height;     //高度
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    //设置默认的压缩设置
    jpeg_set_defaults(&cinfo);
    //设置用户定义的压缩质量
    jpeg_set_quality(&cinfo, quality, TRUE);
    //开始 JPEG 压缩过程
    jpeg_start_compress(&cinfo, TRUE);
    //用于存储单行 RGB 数据的数组
    JSAMPROW row_pointer[1];
    row_pointer[0] = (JSAMPROW)malloc(3 * region_width);
    //遍历指定区域的每一行
    for (int i = y; i < y + region_height; i++)
    {
        int j;
        unsigned short *base = buffer + i * WIDTH + x;
        unsigned char *row = row_pointer[0];

        //RGB565----->RGB888
        for (j = 0; j < region_width; j++)
        {
            unsigned short pixel = *base++;
            row[0] = ((pixel >> 11) & 0x1F) << 3; // Red
            row[1] = ((pixel >> 5) & 0x3F) << 2;  // Green
            row[2] = (pixel & 0x1F) << 3;          // Blue
            row += 3;
        }
        //写入扫描行数据到 JPEG 文件
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    //完成 JPEG 压缩
    jpeg_finish_compress(&cinfo);
    //关闭文件
    fclose(file);
    //释放内存
    free(row_pointer[0]);
    // 销毁 JPEG 压缩对象   
    jpeg_destroy_compress(&cinfo);
    return 0;
}

int my_camera(void)
{
    int ts_x,ts_y;

    open_lcd();         //打开lcd
    open_ts();          //打开触摸屏

    while(1)
    {
        get_xy(&ts_x,&ts_y);
        if(ts_x > 820 && ts_x < 1024 && ts_y > 0 && ts_y < 120)
        {
            printf("正在实时监控!\n");
            v4l2_stream_on();                                           //开启视频采集
            pthread_create(&v4l2_thread, NULL, show_video, NULL);       //创建新线程
            camera_state = 1;
            capture_state = 0;
        }
        if(ts_x > 820 && ts_x < 1024 && ts_y > 370 && ts_y < 480)
        {
            printf("拍照!\n");
            jpg_num++;
            if(jpg_num > 10)
            {
                printf("已到最大抓拍数量!!!\n");
                jpg_num = 1;
            }
            v4l2_stream_on();                                           //开启视频采集
            pthread_create(&v4l2_thread, NULL, show_video, NULL);       //创建新线程
            camera_state = 1;
            capture_state = 1;
        }
        if(ts_x > 820 && ts_x < 1024 && ts_y > 490 && ts_y < 600)
        {
            printf("\n======Exit======\n");
            camera_state = 0;
            v4l2_stream_off();                      //关闭视频采集
            show_bmp_image("./gongneng.bmp",0,0);
            capture_state = 0;
            break;
        }
    }
    close_lcd();
	return 0;
}
