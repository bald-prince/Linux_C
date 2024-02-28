#ifndef __LED_H
#define __LED_H

//触发模式文件路径
#define LED_TRIGGER     "/sys/class/leds/sys-led/trigger"
//常亮模式文件路径
#define LED_BRIGHTNESS  "/sys/class/leds/sys-led/brightness"
//打印用例
#define USAGE()         fprintf(stderr,"usage:\n %s <on|off>\n %s <trigger> <type>\n", argv[0], argv[0])

#endif 

