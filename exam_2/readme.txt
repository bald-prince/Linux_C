一、使用前提
    完成了LCD和触摸屏的驱动,将触摸屏驱动ft5x06.ko拷贝至跟文件系统/lib/modules/4.1.15下
    cp EXAM_2/bin/exam_2 /home/ycl/linux/nfs/rootfs/lib/modules/4.1.15

二、使用方法
    1.进入工程目录
        cd exam_2
    2.将工程目录下的install.sh拷贝至根文件系统/lib/modules/4.1.15
        cp exam_2/install.sh /home/ycl/linux/nfs/rootfs/lib/modules/4.1.15
    3.进行编译
        ./exam_2.sh
    5.将编译后的可执行文件拷贝至/lib/modules/4.1.15
        cp bin/exam_2 /home/ycl/linux/nfs/rootfs/lib/modules/4.1.15
    4.安装驱动
        ./install.sh
    5.运行程序
        ./exam_2
