ifneq ($(KERNELRELEASE),)
	obj-m := workqueue_test.o
else
	KERNELDIR ?= /usr/src/kernels/3.10.0-1160.53.1.el7.x86_64
	PWD := $(shell pwd)
	CONFIG_MODULE_SIG=n
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
.PHONY:clean
clean:
	-rm *.mod.c *.o *.order *.symvers *.ko
