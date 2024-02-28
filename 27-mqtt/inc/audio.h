#ifndef __AUDIO_H
#define __AUDIO_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <alsa/asoundlib.h>
#include <errno.h>
#include <string.h>
#include "ts.h"

#define PCM_PLAYBACK_DEV    "hw:0,0"

/************************************
WAV 音频文件解析相关数据结构申明
************************************/
typedef struct WAV_RIFF {
    char ChunkID[4];                /* "RIFF" */
    u_int32_t ChunkSize;            /* 从下一个地址开始到文件末尾的总字节数 */
    char Format[4];                 /* "WAVE" */
} __attribute__ ((packed)) RIFF_t;

typedef struct WAV_FMT {
    char Subchunk1ID[4];            /* "fmt " */
    u_int32_t Subchunk1Size;        /* 16 for PCM */
    u_int16_t AudioFormat;          /* PCM = 1*/
    u_int16_t NumChannels;          /* Mono = 1, Stereo = 2, etc. */
    u_int32_t SampleRate;           /* 8000, 44100, etc. */
    u_int32_t ByteRate;             /* = SampleRate * NumChannels * BitsPerSample/8 */
    u_int16_t BlockAlign;           /* = NumChannels * BitsPerSample/8 */
    u_int16_t BitsPerSample;        /* 8bits, 16bits, etc. */
} __attribute__ ((packed)) FMT_t;


typedef struct WAV_DATA {
    char Subchunk2ID[4];            /* "data" */
    u_int32_t Subchunk2Size;        /* data size */
} __attribute__ ((packed)) DATA_t;

//函数声明
int snd_pcm_init_play(void);
int open_wav_file(const char *file);
int paly_audio(const char *file);
int capture_audio(const char *filename);
int snd_pcm_init_capture(void);
void snd_playback_async_callback(snd_async_handler_t *handler);
int snd_pcm_init_paly_async(void);
int paly_audio_async(const char *file);
int snd_pcm_init_capture_async(void);
void snd_capture_async_callback(snd_async_handler_t *handler);
int capture_audio_async(const char *file);
#endif // !__AUDIO_H



