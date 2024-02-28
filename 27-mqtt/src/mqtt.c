#include "mqtt.h"

int on_message(void *context, char *topicName, int topicLen, MQTTClient_message *message) 
{
    char *payload = message->payload;
    if (!strcmp(topicName, SUBSCRIBE_TOPIC)) 
    {
        //解析json数据
        struct json_object *json_obj = json_tokener_parse(payload);
        //获取down_data数组
        struct json_object *down_data_array;
        json_object_object_get_ex(json_obj, "down_data", &down_data_array);

        //遍历down_data数组
        int array_len = json_object_array_length(down_data_array);
        for (int i = 0;i < array_len;i++)
        {
            struct json_object *down_data_entry = json_object_array_get_idx(down_data_array,i);

            //获取time字段
            struct json_object *time_obj;
            if(json_object_object_get_ex(down_data_entry,"time",&time_obj))
            {
                const char *time_str = json_object_get_string(time_obj);
                printf("time:%s\n",time_str);
            }

            // 获取Current1-Current8字段
            struct json_object *current_obj;
            if (json_object_object_get_ex(down_data_entry, "Current1-Current8", &current_obj)) 
            {
                int current_len = json_object_array_length(current_obj);
                printf("Current1-Current8: ");
                for (int j = 0; j < current_len; j++) 
                {
                    double current_value = json_object_get_double(json_object_array_get_idx(current_obj, j));
                    printf("%.3f ", current_value);
                }
                printf("\n");
            }
        }
        //释放JSON对象
        json_object_put(json_obj);
    }
    /*printf("%s\n", payload);
    if (!strcmp(topicName, SUBSCRIBE_TOPIC)) 
    { 
        //校验消息的主题
        if (!strcmp("2", message->payload)) //如果接收到的消息是"2"则设置 LED 为呼吸灯模式
            system("echo heartbeat > /sys/class/leds/sys-led/trigger");
        if (!strcmp("1", message->payload)) 
        { 
            //如果是"1"则 LED 常量
            system("echo none > /sys/class/leds/sys-led/trigger");
            system("echo 1 > /sys/class/leds/sys-led/brightness");
        }
        else if (!strcmp("0", message->payload)) 
        {
            //如果是"0"则 LED 熄灭
            system("echo none > /sys/class/leds/sys-led/trigger");
            system("echo 0 > /sys/class/leds/sys-led/brightness");
        }
        //接收到其它数据 不做处理
    }*/

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 0;
}

int mqtt_choose(char *topic,char *payload,int state)
{
    int rc;
    MQTTClient client;

    //创建 mqtt 客户端对象
    rc = MQTTClient_create(&client, BROKER_ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) 
    {
        printf("Failed to create client, return code %d\n", rc);
        rc = EXIT_FAILURE;
        return -1;
    }

    //设置接收回调
    if(state == SUBSCRIBE)
    {
        rc = MQTTClient_setCallbacks(client,NULL,NULL,on_message,NULL);
        if (rc != MQTTCLIENT_SUCCESS) 
        {
            printf("Failed to set callbacks, return code %d\n", rc);
            rc = EXIT_FAILURE;
            MQTTClient_destroy(&client);
            return -1;
        }
    }
    
    //连接mqtt服务器
    //1.mqtt连接参数
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 30;           //心跳包间隔时间
    conn_opts.cleansession = 0;                 //cleanSession 标志
    conn_opts.username = USERNAME;              //用户名
    conn_opts.password = PASSWORD;              //密码
    //2.连接到mqtt服务器
    rc = MQTTClient_connect(client, &conn_opts);
    if (rc != MQTTCLIENT_SUCCESS) 
    {
        printf("Failed to connect, return code %d\n", rc);
        rc = EXIT_FAILURE;
        MQTTClient_destroy(&client);
        return -1;
    }
    //printf("MQTT服务器连接成功!\n");

    //state = 1,发布消息
    if(state == PUBLISH)
    {
        MQTTClient_message message = MQTTClient_message_initializer;
        message.payload = payload;
        message.payloadlen = strlen(payload);
        message.qos = QOS;
        message.retained = 1;
        MQTTClient_deliveryToken token;
        rc = MQTTClient_publishMessage(client, topic, &message, &token);
        if ( rc != MQTTCLIENT_SUCCESS)
        {
            printf("Failed to publish message, return code %d\n", rc);
            rc = EXIT_FAILURE;
            MQTTClient_unsubscribe(client, topic);
            return -1;
        }  
        MQTTClient_waitForCompletion(client, token, TIMEOUT);
    }

    //state = 2,订阅消息
    if(state == SUBSCRIBE)
    {
        rc = MQTTClient_subscribe(client, topic, QOS);
        if ( rc != MQTTCLIENT_SUCCESS)
        {
            printf("Failed to subscribe, return code %d\n", rc);
            rc = EXIT_FAILURE;
            rc = MQTTClient_disconnect(client, TIMEOUT);
            if(rc != MQTTCLIENT_SUCCESS)
                printf("Failed to disconnect, return code %d\n", rc);
            return -1;
        }
    }
    return 0;
}




