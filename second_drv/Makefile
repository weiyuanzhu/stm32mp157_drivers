# Makefile for external out-of-tree Linux kernel module

# Object file(s) to be built
obj-m := second_drv.o

# Path to the directory that contains the Linux kernel source code
# and the configuration file (.config)
KERNEL_DIR = /home/weiyuan/STM32MPU_workspace/STM32CubeIDE/STM32MP157C-DK2/CA7/linux-6.1.28/build

# Path to the directory that contains the generated objects
DESTDIR ?= ./

# Path to the directory that contains the source file(s) to compile
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules
        

install:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) INSTALL_MOD_PATH=$(DESTDIR) modules_install

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean