# kernel路徑
KERNELDIR := /lib/modules/6.12.25+rpt-rpi-2712/build
# 當前路徑
CURRENT_PATH := $(shell pwd)
# 要編譯的文件
obj-m := newchrled.o
# 表示我們編譯的是模塊
build: kernel_modules

# -C: 當前的工作目錄切換到指定目錄中，也就是 KERNELDIR 目錄
# M: 模塊源碼目錄
# 後面的 modules 表示編譯模塊
# clean: 清除make完後的編譯檔案
kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

# 放到這目錄 sudo cp <your filename>.ko /lib/modules/$(uname -r)/kernel/drivers/misc/
# 第一次加入要用 sudo depmod -a
# sudo modprobe <your filename> //不要加.ko