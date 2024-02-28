#include "ts.h"
#include "led.h"
#include "lcd.h"
#include "camera.h"
#include "display.h"
#include <unistd.h>
#include "function.h"
#include "audio.h"
#include "socket.h"

int main(int argc,char *argv[])
{
 
    pthread_t thread;
    pthread_create(&thread, NULL,listen_socket_data,NULL);
    while (1)
    {

    }

    pthread_join(thread, NULL);
    return 0;
}
