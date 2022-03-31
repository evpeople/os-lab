ifneq ($(KERNELRELEASE),)
	obj-m := sum_time.o
else
	KERNELDIR ?= /usr/src/kernels/3.10.0-1160.53.1.el7.x86_64
	PWD := $(shell pwd)
default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
.PHONY:clean
clean:
	-rm *.mod.c *.o *.order *.symvers *.ko
