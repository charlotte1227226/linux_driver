# 讓它自動抓目前正在跑的 kernel 版本
KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# 你的驅動檔名要叫 dtsled.c
obj-m := dtsled.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
