## 题目 内核模块编程：打印当前CPU负载

## 实验目的

1. 了解linux 的proc文件系统功能。
2. 掌握内核模块编程的基本能力。
3. 掌握在内核中读写文件数据的方法（数据结构和函数接口等）。

## 实验内容

​	使用内核模块编程的方法从系统中获取1min内当前cpu的负载，并将其打印出来。

## 实验设计原理和步骤

### 原理方法

​	路径 /proc/下的系统文件loadavg中存有近期的cpu平均负载。

​	如图，前面三个数字分别是cpu在1分钟、5分钟、15分钟内的平均负载。

​	因此在代码中读取该文件内容从而获取平均负载即可。

![image-20220324225759058](doc1.assets/image-20220324225759058.png)

### 代码步骤

​	代码的核心部分包括：

1. 将文件/proc/loadavg打开（get_loadavg函数完成）；

2. 读取文件最开头的数据，即1min内平均负载（get_loadavg函数完成）；

3. 将数据保存后关闭文件（get_loadavg函数完成）；

4. 通过printk函数打印（cpu_loadavg_init函数完成）；

   然后cpu_loadavg_init函数作为模块的init函数，在模块被加载时调用。

   详细代码见后文程序代码部分。

## 实验结果及分析

编译完成

![image-20220324225849052](doc1.assets/image-20220324225849052.png)

从加载模块到卸载模块

![image-20220324225812775](doc1.assets/image-20220324225812775.png)

单看dmesg输出

![image-20220324225934597](doc1.assets/image-20220324225934597.png)

## 程序代码

#### 模块源文件

 [lab2.c](src1\lab2.c) 

```c
#include <linux/module.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

char tmp_cpu_load[5] = {'\0'};

static int get_loadavg(void)
{
	struct file *fp_cpu;
	loff_t pos = 0;
	char buf_cpu[10];
	fp_cpu = filp_open("/proc/loadavg", O_RDONLY, 0);
	if (IS_ERR(fp_cpu)){
		printk("Failed to open loadavg file!\n");
		return -1;
	}
	kernel_read(fp_cpu, buf_cpu, sizeof(buf_cpu), &pos);
	strncpy(tmp_cpu_load, buf_cpu, 4);
	filp_close(fp_cpu, NULL);
	return 0;
}

static int __init cpu_loadavg_init(void)
{
	printk("Start cpu_loadavg!\n");
	if(0 != get_loadavg()){
		printk("Failed to read loadarvg file!\n");
		return -1;
	}
	printk("The cpu loadavg in one minute is: %s\n", tmp_cpu_load);
	return 0;
}

static void __exit cpu_loadavg_exit(void)
{
	printk("Exit cpu_loadavg!\n");
}

module_init(cpu_loadavg_init);
module_exit(cpu_loadavg_exit);
```

#### Makefile文件

 [Makefile](src1\Makefile) 

```makefile
ifneq ($(KERNELRELEASE),)
	obj-m := lab2.o
else
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

```

