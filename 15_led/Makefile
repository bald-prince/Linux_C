# Makefile for the C project

# 编译器设置
CC = arm-poky-linux-gnueabi-gcc -march=armv7ve -mfpu=neon -mfloat-abi=hard -mcpu=cortex-a7 --sysroot=/opt/fsl-imx-x11/4.1.15-2.1.0/sysroots/cortexa7hf-neon-poky-linux-gnueabi
# 编译参数设置
CFLAGS = -Wall -I./inc

# 源文件和目标文件目录
SRCDIR = src
INCDIR = inc
OBJDIR = obj
BINDIR = bin

# 获取所有的源文件和目标文件
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

# 目标文件
TARGET = $(BINDIR)/ledApp

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# 生成目标文件
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

# PHONY规则，防止与实际文件同名的规则冲突
.PHONY: all clean
