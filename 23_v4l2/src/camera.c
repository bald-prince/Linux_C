#include "camera.h"

 int v4l2_fd; //摄像头设备文件描述符

static cam_buf_info buf_infos[FRAMEBUFFER_COUNT];
static int frm_width, frm_height; //视频帧宽度和高度

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
    /*** 判断是否已经设置为我们要求的 RGB565 像素格式
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
            fprintf(stderr, "ioctl error: VIDIOC_QBUF: %s\n", strerror(errno));
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
        fprintf(stderr, "ioctl error: VIDIOC_STREAMON: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

volatile int v4l2_state = 1;

void *show_video(void *arg)
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
    while(v4l2_state)
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
    pthread_exit(NULL);
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
    while(v4l2_state)
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


