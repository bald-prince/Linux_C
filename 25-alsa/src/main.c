#include "ts.h"
#include "led.h"
#include "lcd.h"
#include "camera.h"
#include "display.h"
#include <unistd.h>
#include "function.h"
//#include "uart.h"
#include "audio.h"

int main(int argc,char *argv[])
{
    capture_audio_async("./test.wav");
    //paly_audio_async("./0.wav");

    return 0;
}
