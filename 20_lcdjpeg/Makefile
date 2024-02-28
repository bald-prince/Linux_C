# 定义编译器
CC = arm-poky-linux-gnueabi-gcc -march=armv7ve -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi

# 编译参数设置
CFLAGS = 	-Wall -I ./inc \
			-I /home/ycl/tools/tslib/include \
			-I /home/ycl/tools/jpeg/include	\
			-I /home/ycl/tools/png/include

# 链接参数设置
LDFLAGS = 	-L /home/ycl/tools/tslib/lib -lts \
			-L /home/ycl/tools/jpeg/lib -ljpeg	\
			-L /home/ycl/tools/png/lib \
			-L /home/ycl/tools/zlib/lib -lpng -lz

# 定义源文件目录和目标文件目录
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# 通过通配符获取源文件目录下的所有.c文件
SOURCES = $(wildcard $(SRC_DIR)/*.c) 

# 将源文件列表中的.c文件替换成.o文件，并将路径指向目标文件目录
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# 定义最终的可执行文件名
TARGET = $(BIN_DIR)/lcdpngApp

# 默认目标
all:clean $(TARGET)

# 链接目标，依赖所有的.o文件
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

# 编译源文件到目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 定义clean规则，删除之前的编译产出
clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)


