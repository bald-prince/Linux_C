#include "audio.h"

/************************************
static 静态全局变量定义
************************************/
static FMT_t wav_fmt;
static snd_pcm_t *pcm = NULL;                       //pcm 句柄
static unsigned int buf_bytes;                      //应用程序缓冲区的大小（字节为单位）
static void *buf = NULL;                            //指向应用程序缓冲区的指针
int wav_fd;
static snd_pcm_uframes_t period_size = 1024;        //周期大小（单位: 帧）
static unsigned int periods = 16;                   //周期数（设备驱动层 buffer 的大小）

int snd_pcm_init_play(void)
{
    snd_pcm_hw_params_t *hw_params = NULL;
    int ret;

    //打开pcm
    ret = snd_pcm_open(&pcm,PCM_PLAYBACK_DEV,SND_PCM_STREAM_PLAYBACK,0);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_open error: %s: %s\n",PCM_PLAYBACK_DEV, snd_strerror(ret));
        return -1;
    }
    //实例化hw_params对象
    snd_pcm_hw_params_malloc(&hw_params);

    //获取PCM设备当前硬件配置，对hw_params进行初始化
    ret = snd_pcm_hw_params_any(pcm,hw_params);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_any error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置访问类型：交错模式
    ret = snd_pcm_hw_params_set_access(pcm,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED); 
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_access error: %s\n", snd_strerror(ret));
        goto err2;
    }

    //设置数据格式: 有符号 16 位、小端模式
    ret = snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_format error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置采样率 
    ret = snd_pcm_hw_params_set_rate(pcm, hw_params, wav_fmt.SampleRate, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_rate error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置声道数: 双声道
    ret = snd_pcm_hw_params_set_channels(pcm, hw_params, wav_fmt.NumChannels);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_channels error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置周期大小: period_size
    ret = snd_pcm_hw_params_set_period_size(pcm, hw_params, period_size, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_period_size error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置周期数（驱动层 buffer 的大小） : periods
    ret = snd_pcm_hw_params_set_periods(pcm, hw_params, periods, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_periods error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 使配置生效 */
    ret = snd_pcm_hw_params(pcm, hw_params);
    snd_pcm_hw_params_free(hw_params); //释放 hwparams 对象占用的内存
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params error: %s\n", snd_strerror(ret));
        goto err1;
    }
    buf_bytes = period_size * wav_fmt.BlockAlign; //变量赋值，一个周期的字节大小
    return 0;

    err2:
        snd_pcm_hw_params_free(hw_params); //释放内存
    err1:
        snd_pcm_close(pcm); //关闭 pcm 设备

    return -1;
}

int open_wav_file(const char *file)
{
    RIFF_t wav_riff;
    DATA_t wav_data;
    int ret;
    wav_fd = open(file, O_RDONLY);
    if (0 > wav_fd) 
    {
        perror("open aduio error");
        return -1;
    }
    //读取 RIFF chunk
    ret = read(wav_fd, &wav_riff, sizeof(RIFF_t));
    if (sizeof(RIFF_t) != ret) 
    {
        if (0 > ret)
            perror("read error");
        else
            fprintf(stderr, "check error: %s\n", file);
        close(wav_fd);
        return -1;
    }
    if (strncmp("RIFF", wav_riff.ChunkID, 4) || strncmp("WAVE", wav_riff.Format, 4)) 
    {
        fprintf(stderr, "check error: %s\n", file);
        close(wav_fd);
        return -1;
    }
    //读取 sub-chunk-fmt
    ret = read(wav_fd, &wav_fmt, sizeof(FMT_t));
    if (sizeof(FMT_t) != ret) 
    {
        if (0 > ret)
            perror("read error");
        else
            fprintf(stderr, "check error: %s\n", file);
        close(wav_fd);
        return -1;
    }
    if (strncmp("fmt ", wav_fmt.Subchunk1ID, 4)) 
    {   
        //校验
        fprintf(stderr, "check error: %s\n", file);
        close(wav_fd);
        return -1;
    }
    //打印音频文件的信息
    printf("<<<<音频文件格式信息>>>>\n\n");
    printf(" file name: %s\n", file);
    printf(" Subchunk1Size: %u\n", wav_fmt.Subchunk1Size);
    printf(" AudioFormat: %u\n", wav_fmt.AudioFormat);
    printf(" NumChannels: %u\n", wav_fmt.NumChannels);
    printf(" SampleRate: %u\n", wav_fmt.SampleRate);
    printf(" ByteRate: %u\n", wav_fmt.ByteRate);
    printf(" BlockAlign: %u\n", wav_fmt.BlockAlign);
    printf(" BitsPerSample: %u\n\n", wav_fmt.BitsPerSample);
    //sub-chunk-data
    if (0 > lseek(wav_fd, sizeof(RIFF_t) + 8 + wav_fmt.Subchunk1Size,SEEK_SET)) 
    {
        perror("lseek error");
        close(wav_fd);
        return -1;
    }
    while(sizeof(DATA_t) == read(wav_fd, &wav_data, sizeof(DATA_t))) 
    {
        //找到 sub-chunk-data
        if (!strncmp("data", wav_data.Subchunk2ID, 4))//校验
            return 0;
        if (0 > lseek(wav_fd, wav_data.Subchunk2Size, SEEK_CUR)) 
        {
            perror("lseek error");
            close(wav_fd);
            return -1;
        }
    }
    fprintf(stderr, "check error: %s\n", file);
    return -1;
}

int paly_audio(const char *file)
{
    int ret;
    //打开wav文件
    if(open_wav_file(file))
        return -1;

    if(snd_pcm_init_play())
    {
        close(wav_fd); //关闭打开的音频文件
        return -1;
    }

    /* 申请读缓冲区 */
    buf = malloc(buf_bytes);
    if (NULL == buf) 
    {
        perror("malloc error");
        snd_pcm_close(pcm); //关闭 pcm 设备
    }
    //播放
    for ( ; ; ) 
    {
        memset(buf, 0x00, buf_bytes); //buf 清零
        ret = read(wav_fd, buf, buf_bytes); //从音频文件中读取数据
        if (0 >= ret) // 如果读取出错或文件读取完毕
            free(buf); //释放内存
        ret = snd_pcm_writei(pcm, buf, period_size);
        if (0 > ret) 
        {
            fprintf(stderr, "snd_pcm_writei error: %s\n", snd_strerror(ret));
            free(buf); //释放内存
        }
        else if (ret < period_size) 
        {
            //实际写入的帧数小于指定的帧数
            //此时我们需要调整下音频文件的读位置
            //将读位置向后移动（往回移） (period_size-ret)*frame_bytes 个字节
            //frame_bytes 表示一帧的字节大小
            if (0 > lseek(wav_fd, (ret-period_size) * wav_fmt.BlockAlign, SEEK_CUR)) 
            {
                perror("lseek error");
                free(buf); //释放内存
            }
        }
    }
    return 0;
}

int snd_pcm_init_capture(void)
{
    snd_pcm_hw_params_t *hw_params = NULL;
    int ret;

    //打开pcm
    ret = snd_pcm_open(&pcm,PCM_PLAYBACK_DEV,SND_PCM_STREAM_CAPTURE,0);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_open error: %s: %s\n",PCM_PLAYBACK_DEV, snd_strerror(ret));
        return -1;
    }
    //实例化hw_params对象
    snd_pcm_hw_params_malloc(&hw_params);

    //获取PCM设备当前硬件配置，对hw_params进行初始化
    ret = snd_pcm_hw_params_any(pcm,hw_params);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_any error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置访问类型：交错模式
    ret = snd_pcm_hw_params_set_access(pcm,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED); 
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_access error: %s\n", snd_strerror(ret));
        goto err2;
    }

    //设置数据格式: 有符号 16 位、小端模式
    ret = snd_pcm_hw_params_set_format(pcm, hw_params, SND_PCM_FORMAT_S16_LE);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_format error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置采样率 
    ret = snd_pcm_hw_params_set_rate(pcm, hw_params, 44100, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_rate error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置声道数: 双声道
    ret = snd_pcm_hw_params_set_channels(pcm, hw_params, 2);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_channels error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置周期大小: period_size
    ret = snd_pcm_hw_params_set_period_size(pcm, hw_params, period_size, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_period_size error: %s\n", snd_strerror(ret));
        goto err2;
    }
    //设置周期数（驱动层 buffer 的大小） : periods
    ret = snd_pcm_hw_params_set_periods(pcm, hw_params, periods, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_periods error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 使配置生效 */
    ret = snd_pcm_hw_params(pcm, hw_params);
    snd_pcm_hw_params_free(hw_params); //释放 hwparams 对象占用的内存
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params error: %s\n", snd_strerror(ret));
        goto err1;
    }
    return 0;
    err2:
        snd_pcm_hw_params_free(hw_params); //释放内存
    err1:
        snd_pcm_close(pcm); //关闭 pcm 设备
        return -1;
    
}

int capture_audio(const char *filename)
{
    unsigned char *buf = NULL;    
    int fd = -1;
    int ret;

    if(snd_pcm_init_capture())
        return -1;

    //申请读缓冲区
    buf_bytes = period_size * 4; //字节大小 = 周期大小*帧的字节大小 16 位双声道
    buf = malloc(buf_bytes);
    if (NULL == buf) 
    {
        perror("malloc error");
        snd_pcm_close(pcm); //关闭 pcm 设备
        return -1;
    }
    
    fd = open(filename, O_WRONLY | O_CREAT | O_EXCL);
    if (0 > fd)
    {
        perror("open aduio error");
        free(buf); //释放内存
        return -1;
    }
    /* 录音 */
    for ( ; ; ) 
    {
        ret = snd_pcm_readi(pcm, buf, period_size);//读取 PCM 数据 一个周期
        if (0 > ret) 
        {
            fprintf(stderr, "snd_pcm_readi error: %s\n", snd_strerror(ret));
            close(fd); //关闭文件
            return -1;
        }
        ret = write(fd, buf, ret * 4); //将读取到的数据写入文件中
        if (0 >= ret)
        {
            close(fd); //关闭文件
            return -1;
        }
    }
    return 0;
}

void snd_playback_async_callback(snd_async_handler_t *handler)
{
    snd_pcm_t *handle = snd_async_handler_get_pcm(handler);//获取PCM句柄
    snd_pcm_sframes_t avail;
    int ret;

    avail = snd_pcm_avail_update(handle);//获取环形缓冲区中有多少帧数据需要填充
    while (avail >= period_size) 
    {  
        //我们一次写入一个周期
        memset(buf, 0x00, buf_bytes);   //buf清零
        ret = read(wav_fd, buf, buf_bytes);
        if (0 >= ret)
            goto out;

        ret = snd_pcm_writei(handle, buf, period_size);
        if (0 > ret) {
            fprintf(stderr, "snd_pcm_writei error: %s\n", snd_strerror(ret));
            goto out;
        }
        else if (ret < period_size) 
        {
            //实际写入的帧数小于指定的帧数
            //此时我们需要调整下音频文件的读位置 重新读取没有播放出去的数据
            //将读位置向后移动（往回移）(period_size-ret)*frame_bytes个字节
            //frame_bytes表示一帧的字节大小
            if (0 > lseek(wav_fd, (ret-period_size) * wav_fmt.BlockAlign, SEEK_CUR)) 
            {
                perror("lseek error");
                goto out;
            }
        }
        avail = snd_pcm_avail_update(handle);   //再次获取、更新avail
    }
    return;
out:
    snd_pcm_close(handle); //关闭pcm设备
    free(buf);
    close(wav_fd);      //关闭打开的音频文件
    exit(EXIT_FAILURE); //退出程序
}

int snd_pcm_init_paly_async(void)
{
    snd_pcm_hw_params_t *hwparams = NULL;
    snd_async_handler_t *async_handler = NULL;
    int ret;

    /* 打开PCM设备 */
    ret = snd_pcm_open(&pcm, PCM_PLAYBACK_DEV, SND_PCM_STREAM_PLAYBACK, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_open error: %s: %s\n",PCM_PLAYBACK_DEV, snd_strerror(ret));
        return -1;
    }

    /* 实例化hwparams对象 */
    snd_pcm_hw_params_malloc(&hwparams);

    /* 获取PCM设备当前硬件配置,对hwparams进行初始化 */
    ret = snd_pcm_hw_params_any(pcm, hwparams);
    if (0 > ret) {
        fprintf(stderr, "snd_pcm_hw_params_any error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /************** 
     设置参数
    ***************/
    /* 设置访问类型: 交错模式 */
    ret = snd_pcm_hw_params_set_access(pcm, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (0 > ret) {
        fprintf(stderr, "snd_pcm_hw_params_set_access error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 设置数据格式: 有符号16位、小端模式 */
    ret = snd_pcm_hw_params_set_format(pcm, hwparams, SND_PCM_FORMAT_S16_LE);
    if (0 > ret) {
        fprintf(stderr, "snd_pcm_hw_params_set_format error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 设置采样率 */
    ret = snd_pcm_hw_params_set_rate(pcm, hwparams, wav_fmt.SampleRate, 0);
    if (0 > ret) {
        fprintf(stderr, "snd_pcm_hw_params_set_rate error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 设置声道数: 双声道 */
    ret = snd_pcm_hw_params_set_channels(pcm, hwparams, wav_fmt.NumChannels);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_channels error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 设置周期大小: period_size */
    ret = snd_pcm_hw_params_set_period_size(pcm, hwparams, period_size, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_period_size error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 设置周期数（驱动层环形缓冲区buffer的大小）: periods */
    ret = snd_pcm_hw_params_set_periods(pcm, hwparams, periods, 0);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params_set_periods error: %s\n", snd_strerror(ret));
        goto err2;
    }

    /* 使配置生效 */
    ret = snd_pcm_hw_params(pcm, hwparams);
    snd_pcm_hw_params_free(hwparams);   //释放hwparams对象占用的内存
    if (0 > ret) 
    {
        fprintf(stderr, "snd_pcm_hw_params error: %s\n", snd_strerror(ret));
        goto err1;
    }

    buf_bytes = period_size * wav_fmt.BlockAlign; //变量赋值，一个周期的字节大小

    /* 注册异步处理函数 */
    ret = snd_async_add_pcm_handler(&async_handler, pcm, snd_playback_async_callback, NULL);
    if (0 > ret) 
    {
        fprintf(stderr, "snd_async_add_pcm_handler error: %s\n", snd_strerror(ret));
        goto err1;
    }
    return 0;
err2:
    snd_pcm_hw_params_free(hwparams);   //释放内存
err1:
    snd_pcm_close(pcm); //关闭pcm设备
    return -1;
}

int paly_audio_async(const char *file)
{
    open_ts();
    snd_pcm_sframes_t avail;
    int ret;
    int x,y;

    /* 打开WAV音频文件 */
    if (open_wav_file(file))
        exit(EXIT_FAILURE);

    /* 初始化PCM Playback设备 */
    if (snd_pcm_init_paly_async())
        goto err1;

    /* 申请读缓冲区 */
    buf = malloc(buf_bytes);
    if (NULL == buf) 
    {
        perror("malloc error");
        goto err2;
    }

    /* 播放：先将环形缓冲区填满数据 */
    avail = snd_pcm_avail_update(pcm);  //获取环形缓冲区中有多少帧数据需要填充
    while (avail >= period_size) 
    {  
        //我们一次写入一个周期
        memset(buf, 0x00, buf_bytes);   //buf清零
        ret = read(wav_fd, buf, buf_bytes);
        if (0 >= ret)
            goto err3;

        ret = snd_pcm_writei(pcm, buf, period_size);//向环形缓冲区中写入数据
        if (0 > ret) {
            fprintf(stderr, "snd_pcm_writei error: %s\n", snd_strerror(ret));
            goto err3;
        }
        else if (ret < period_size) 
        {
            //实际写入的帧数小于指定的帧数
            //此时我们需要调整下音频文件的读位置
            //将读位置向后移动（往回移）(period_size-ret)*frame_bytes个字节
            //frame_bytes表示一帧的字节大小
            if (0 > lseek(wav_fd, (ret-period_size) * wav_fmt.BlockAlign, SEEK_CUR)) 
            {
                perror("lseek error");
                goto err3;
            }
        }
        avail = snd_pcm_avail_update(pcm);  //再次获取、更新avail
    }
    for(;;)
    {
        get_xy(&x,&y);
        printf("x: %d, y: %d\n",x,y);
    }
err3:
    free(buf);     //释放内存
err2:
    snd_pcm_close(pcm); //关闭pcm设备
err1:
    close(wav_fd);      //关闭打开的音频文件
    exit(EXIT_FAILURE);
}

void snd_capture_async_callback(snd_async_handler_t *handler)
{
    snd_pcm_t *handle = snd_async_handler_get_pcm(handler);
    snd_pcm_sframes_t avail;
    int ret;

    avail = snd_pcm_avail_update(handle);       //检查有多少帧数据可读
    while (avail >= period_size) 
    {
        //每次读取一个周期
        ret = snd_pcm_readi(handle,buf,period_size);
        if(0 > ret)
        {
            fprintf(stderr,"snd_pcm_readi error:%s\n",snd_strerror(ret));       //读取PCM数据 一个周期
            goto out;
        }
        ret = write(wav_fd, buf,ret * 4);       //将读取到的数据写入文件中
        if(0 >= ret)
            goto out;

        avail = snd_pcm_avail_update(handle);   ////再次读取、更新avail
    }
out:
    snd_pcm_close(handle);
    free(buf);
    close(wav_fd);
    exit(EXIT_FAILURE);
}

int snd_pcm_init_capture_async(void)
{
    snd_pcm_hw_params_t *hw_params = NULL;   
    snd_async_handler_t *async_handler = NULL;
    int ret;
    //打开PCM设备
    ret = snd_pcm_open(&pcm,PCM_PLAYBACK_DEV,SND_PCM_STREAM_CAPTURE,0);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_open failed:%s:%s\n",PCM_PLAYBACK_DEV,snd_strerror(ret));
        return -1;
    }
    //实例化hwparams对象
    snd_pcm_hw_params_malloc(&hw_params);
    //获取PCM设备当前硬件配置,对hwparams进行初始化
    ret = snd_pcm_hw_params_any(pcm,hw_params);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_any failed:%s\n",snd_strerror(ret));
        goto err2;
    }

    /************** 
     设置参数
    ***************/
    //设置访问类型: 交错模式
    ret = snd_pcm_hw_params_set_access(pcm,hw_params,SND_PCM_ACCESS_RW_INTERLEAVED);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_access failed:%s\n",snd_strerror(ret));
        goto err2;
    }
    //设置数据格式: 有符号16位、小端模式
    ret = snd_pcm_hw_params_set_format(pcm,hw_params,SND_PCM_FORMAT_S16_LE);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_format failed:%s\n",snd_strerror(ret));
        goto err2;
    }
    //设置采样率
    ret = snd_pcm_hw_params_set_rate(pcm,hw_params,44100,0);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_rate failed:%s\n",snd_strerror(ret));
        goto err2;
    }
    //设置声道数: 双声道
    ret = snd_pcm_hw_params_set_channels(pcm,hw_params,2);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_channels failed:%s\n",snd_strerror(ret));
        goto err2;
    }
    //设置周期大小: period_size
    ret = snd_pcm_hw_params_set_period_size(pcm,hw_params,period_size,0);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_period_size failed:%s\n",snd_strerror(ret));
        goto err2;
    }
    //设置周期数（buffer的大小）: periods
    ret = snd_pcm_hw_params_set_periods(pcm,hw_params,periods,0);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params_set_periods failed:%s\n",snd_strerror(ret));
        goto err2;
    }
    //使配置生效
    ret = snd_pcm_hw_params(pcm,hw_params);
    snd_pcm_hw_params_free(hw_params);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_hw_params failed:%s\n",snd_strerror(ret));
        goto err1;
    }
    //注册异步处理函数
    ret = snd_async_add_pcm_handler(&async_handler,pcm,snd_capture_async_callback,NULL);
    if(0 > ret)
    {
        fprintf(stderr, "snd_async_add_handler failed:%s\n",snd_strerror(ret));
        goto err1;
    }
    return 0;
err2:
    snd_pcm_hw_params_free(hw_params);
err1:
    snd_pcm_close(pcm);
    return -1;
}

int capture_audio_async(const char *file)
{
    open_ts();
    int ret,x,y;
    if(snd_pcm_init_capture_async())
        exit(EXIT_FAILURE);

    buf_bytes = period_size * 4;
    buf = malloc(buf_bytes);
    if(NULL == buf)
    {
        perror("malloc error");
        goto err1;
    }

    int fd = open(file,O_WRONLY | O_CREAT | O_EXCL);
    if(-1 == fd)
    {
        perror("open error");
        goto err2;
    }

    ret = snd_pcm_start(pcm);
    if(0 > ret)
    {
        fprintf(stderr, "snd_pcm_start failed:%s\n",snd_strerror(ret));
        goto err3;
    }

    for(;;)
    {
        get_xy(&x,&y);
        printf("x: %d, y: %d\n",x,y);
        //sleep(1);
    }
    return 0;
err3:
    close(fd);
err2:
    free(buf);
err1:
    snd_pcm_close(pcm);
    exit(EXIT_FAILURE);
}





