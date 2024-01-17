
# 1. 使用不同的开发板内核时, 一定要修改KERN_DIR
# 2. KERN_DIR中的内核要事先配置、编译, 为了能编译内核, 要先设置下列环境变量:
# 2.1 ARCH,          比如: export ARCH=arm64
# 2.2 CROSS_COMPILE, 比如: export CROSS_COMPILE=aarch64-linux-gnu-
# 2.3 PATH,          比如: export PATH=$PATH:/home/book/100ask_roc-rk3399-pc/ToolChain-6.3.1/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin 
# 注意: 不同的开发板不同的编译器上述3个环境变量不一定相同,
#       请参考各开发板的高级用户使用手册


# Makefile for external out-of-tree Linux kernel module

# Object file(s) to be built
# obj-m	+= dali.o

# Path to the directory that contains the Linux kernel source code
# and the configuration file (.config)
# KERNEL_DIR = /home/weiyuan/STM32MPU_workspace/STM32CubeIDE/STM32MP157C-DK2/CA7/linux-6.1.28/build
KERNEL_DIR = /home/weiyuan/workspace/linux-6.1.28/build/

# Path to the directory that contains the generated objects
DESTDIR ?= ./

# Path to the directory that contains the source file(s) to compile
PWD := $(shell pwd)

default:
	# $(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
	$(CC) timelist.c -o send.out
        
install:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) INSTALL_MOD_PATH=$(DESTDIR) modules_install

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean





