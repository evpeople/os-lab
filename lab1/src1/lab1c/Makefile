
ifneq ($(KERNELRELEASE),)
#kbuild syntax. dependency relationshsip of files and target modules are listed here
obj-m += lab1c.o

else
PWD :=$(shell pwd)
KVER ?=$(shell uname -r)
KDIR :=/usr/src/kernels/$(KVER)
all:
	@echo $(MAKE) -C $(KDIR) M=$(PWD)
	@$(MAKE) -C $(KDIR) M=$(PWD)  modules
clean:
	@rm -rf . *.cmd *.o *.mod.c *.ko *.symvers *.ko.unsigned *.order
endif
