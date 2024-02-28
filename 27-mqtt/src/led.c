#include "led.h"

int led_trigger_fd;
int led_brightness_fd;

//1、打开led文件
int open_led_trigger(void)
{
	led_trigger_fd = open(LED_TRIGGER, O_RDWR);
	if (-1 == led_trigger_fd)
	{
		perror("Open error");
		return -1;
	}
	return 0;
}

int open_led_brightness(void)
{
    led_brightness_fd = open(LED_BRIGHTNESS, O_RDWR);
	if (-1 == led_brightness_fd)
	{
		perror("Open error");
		return -1;
	}
	return 0;
}

//2、led操作
void led_switch(char *state)
{
    if(!strcmp(state,"on"))
    {
        write(led_trigger_fd, "none", 4); 	//先将触发模式设置为none
        write(led_brightness_fd, "1", 1); 		//点亮LED
    }
	else if(!strcmp(state,"off"))
	{
		write(led_trigger_fd, "none", 4); 	//先将触发模式设置为none
        write(led_brightness_fd, "0", 1); 		//点亮LED
	}
}

void led_trigger(char *state,char *heartbeat)
{
	if(!strcmp(state,"trigger"))
	{
		write(led_trigger_fd, heartbeat, strlen(heartbeat));
	}
}


//3、关闭led
int close_led(void)
{
	close(led_trigger_fd);
	close(led_brightness_fd);
	return 0;

}