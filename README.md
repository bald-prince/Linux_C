# **正点原子Alpha Linux开发板应用编程学习代码**

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

16_gpio:
