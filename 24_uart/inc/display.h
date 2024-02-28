#ifndef __DISPLAY_H
#define __DISPLAY_H
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <string.h>
#include <jpeglib.h>
#include <png.h>
#include <math.h>
#include <wchar.h>
#include <ft2build.h>
#include "lcd.h"

int show_bmp_image(const char *path,int x,int y);
int display_image(const char *path,unsigned short *lcd_buf);


#endif