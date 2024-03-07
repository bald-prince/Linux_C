# **正点原子Alpha Linux开发板应用编程学习代码**

## 项目环境：

​	ubuntu16.04、正点原子Alpha出厂系统

## 1_IO

​	文件I/O基础，包括打开文件 open、关闭文件 close、写文件 write、读文件 read的基本使用。  

## 3_chapter

​	深入探究I/O，函数返回错误的处理；退出程序 exit()、 _Exit()、 _exit()； 空洞文件的概念；open 函数的 O_APPEND 和 O_TRUNC 标志；多次打开同一文件； 复制文件描述符；文件共享介绍；原子操作与竞争冒险；系统调用 fcntl()和 ioctl()介绍；截断文件。

## 5_chapter

​	文件属性与目录  ,Linux 系统的文件类型； stat 系统调用；文件各种属性介绍：文件属主、访问权限、时间戳；符号链接与硬链接；目录；删除文件与文件重命名。  

## 15_led

​	Linux应用编程操作Alpha开发板上的LED灯。

​	项目目录结构：

​		/bin：编译后的可执行文件。

​		/inc：项目的头文件

​		/obj：编译之后的中间文件

​		/src：项目的源文件

​		install.sh：一键编译就，./install.sh运行

​		Makefile：make脚本，用于编译

## 16_gpio

​	Linux应用编程控制 GPIO， 譬如控制 GPIO 输出高电平、或输出低电平 。项目包括源码15_led

​	项目目录结构：

​		/bin：编译后的可执行文件。

​		/inc：项目的头文件

​		/obj：编译之后的中间文件

​		/src：项目的源文件

​		install.sh：一键编译就，./install.sh运行

​		Makefile：make脚本，用于编译

## 17_input

​	Linux应用编程触摸屏输入，触摸事件。

​	项目结构同上。

## 18_tslib

​	Linux下利用tslib库驱动触摸屏。

​	项目结构同上。

​	注意事项：需要移植tslib库，版本：1.16，移植完之后，在Makefile下更改CFLAGS、LDFLAGS的路径并将编译后的库文件拷贝至开发板的/usr/lib下进行替换。具体可参考正点原子Linux应用编程手册第十八章。

## 19_framebuffer

​	Linux 下的 Framebuffer 应用编程 ，通过画点的方式实现图片的显示。

​	项目目录结构：

​		/bin：编译后的可执行文件。

​		/inc：项目的头文件

​		/obj：编译之后的中间文件

​		/src：项目的源文件

​		build_project.sh：一键编译就，./install.sh运行

​		Makefile：make脚本，用于编译

## 20_lcdjpeg

​	利用libjpeg库在lcd上显示jpeg图片。

​	项目结构同上。

​	注意事项：需要移植libjpeg库，版本：v9b，移植完之后，在Makefile下更改CFLAGS、LDFLAGS的路径并将编译后的库文件拷贝至开发板的/usr/lib下进行替换。具体可参考正点原子Linux应用编程手册第二十章。

## 21_lcdpng

​	利用libpng库显示png图片。

​	项目结构同上。

​	注意事项：需要移植libpng库，版本：1.6.35，移植完之后，在Makefile下更改CFLAGS、LDFLAGS的路径并将编译后的库文件拷贝至开发板的/usr/lib下进行替换。具体可参考正点原子Linux应用编程手册第二十一章。

## 22_showchar

​	在lcd屏幕上显示字符。

​	项目结构同上。

​	注意事项：需要移植freetype库，版本：2.8，移植完之后，在Makefile下更改CFLAGS、LDFLAGS的路径并将编译后的库文件拷贝至开发板的/usr/lib下进行替换。具体可参考正点原子Linux应用编程手册第二十二章。

## 23_v4l2

​	利用v4l2驱动OV5640摄像头并在lcd上进行显示。

​	项目结构同上。

## 24_uart

​	具体可参考正点原子Linux应用编程手册第二十四章。

## 25_alsa

​	Linux 下的音频应用编程 。

​	项目结构同上。

​	注意事项：需要移植alsa-lib 和alsa-utils 库，这里由于手册上未提供，各位自行百度移植，移植完之后，在Makefile下更改CFLAGS、LDFLAGS的路径并将编译后的库文件拷贝至开发板的/usr/lib下进行替换。具体可参考正点原子Linux应用编程手册第二十五章。

## 26_socket

​	具体可参考正点原子Linux应用编程手册第二十六章。

## 26_mqtt

​	利用MQTT订阅云服务器话题，获取数据，云服务器采用的MQTTX软件，使用教程可自行百度EMQX。并采用json库进行数据解析。

​	项目结构同上。

​	注意事项：需要移植json 库，这里由于手册上未提供，各位自行百度移植，移植完之后，在Makefile下更改CFLAGS、LDFLAGS的路径并将编译后的库文件拷贝至开发板的/usr/lib下进行替换。

## camera

​	linux应用编程的综合工程，功能包括相册、监控、拍照、播放音频。

## ts_control_led

​	一个简单的综合工程，利用触摸屏控制板载LED灯。

​	
