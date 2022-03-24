ifneq ($(KERNELRELEASE),)
	obj-m := lab2.o
else
	#KERNELDIR ?= /root/osLab2
	PWD := $(shell pwd)
	KVER ?=$(shell uname -r)
	KERNELDIR :=/usr/src/kernels/$(KVER)
default:
	@echo $(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif
.PHONY:clean
clean:
	-rm *.mod.c *.o *.order *.symvers *.ko

