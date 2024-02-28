#ifndef __MQTT_H
#define __MQTT_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "led.h"
#include "json.h"

//mqtt服务器参数
#define BROKER_ADDRESS      "121.41.113.243"    //"tcp://broker.emqx.io:1883"
#define CLIENTID            "mqttx_test"
#define USERNAME            "imx6ull"
#define PASSWORD            "19980628"

#define QOS                 0
#define TIMEOUT             10000L
#define PUBLISH             1
#define SUBSCRIBE           2

//话题宏定义
#define SUBSCRIBE_TOPIC             "ycl/subscribe_topic" 
#define PUBLISH_TOPIC               "ycl/publish_topic"
//函数声明
int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message);
int mqtt_choose(char *topic, char *payload,int state);
#endif 
