#include "ts.h"
#include "led.h"
#include "lcd.h"
#include "camera.h"
#include "display.h"
#include <unistd.h>
#include "function.h"
#include "audio.h"
#include "socket.h"
#include "mqtt.h"

int main(int argc,char *argv[])
{
    while (1)
    {
        mqtt_choose(SUBSCRIBE_TOPIC,"HELLO WORLD",SUBSCRIBE);
        sleep(5);
    }
    return 0;
}
