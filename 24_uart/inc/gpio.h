#ifndef __GPIO_H
#define __GPIO_H
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int gpio_config(const char *attr,const char *val,char *gpio_path);


#endif 
