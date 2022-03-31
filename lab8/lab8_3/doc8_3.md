# **题目：内核时间管理——调用内核时钟接口监控运行时间**

# 实验目的

学习使用Linux内核时钟接口的调用。

# 实验内容

1. 调用内核时钟接口，编写内核模块，监控实现累加计算sum=1+2+3+...+100000 所花时间。
2. 加载、卸载模块并查看模块打印信息。

# 实验设计原理

1. timeval结构体 

    头文件：<linux/time.h>
```c
struct timeval { 
         __kernel_time_t         tv_sec;        /* seconds */ 
         __kernel_suseconds_t    tv_usec;        /* microseconds */ 
 };
```
其中tv_sec是自1970年1月1日 00:00:00 起到现在的秒数。而tv_usec是当前秒数已经经过的微秒数。

2. do_gettimeofday()

    头文件：<linux/time.h>
    
    函数原型：void do_gettimeofday(struct timeval *tv);
    
    功能：返回自1970-01-01  00:00:00到现在的秒数，及当前秒经过的毫秒数，保存在tv指向的timeval 结构体中。

3. rtc_time结构体

    头文件<linux/rtc.h>
```c
struct rtc_time {
     int tm_sec;           // 表「秒」数，在[0,61]之间，多出来的两秒是用来处理跳秒问题用的。
     int tm_min;          // 表「分」数，在[0,59]之间。
     int tm_hour;         // 表「时」数，在[0,23]之间。
     int tm_mday;        // 表「本月第几日」，在[1,31]之间。
     int tm_mon;         // 表「本年第几月」，在[0,11]之间。
     int tm_year;         // 要加1900表示那一年。
     int tm_wday;        // 表「本周第几日」，在[0,6]之间。
     int tm_yday;        // 表「本年第几日」，在[0,365]之间，闰年有366日。
     int tm_isdst;        // 表是否为「日光节约时间」。
 };
```
年份加上1900，月份加上1，小时加上8。

4. rtc_time_to_tm()

    头文件：<linux/rtc.h>
    
    函数原型：void rtc_time_to_tm(unsigned long time, struct rtc_time *tm)；

    功能：将time存储的秒数转换为年月日时分秒等信息保存在rtc_time结构体中。
    
    参数：time为秒数，可以是do_gettimeofday()函数获取的秒数。tm是rtc_time结构体指针，结构体中存放了年月日时分秒等信息。

# 实验步骤

编写C程序，程序主体以及测试对象为一个从1到100000的累加for循环。在调用累加函数之前使用模块初始化函数包装它：首先使用gettimeofday以及timeval获取当前系统时间并存储其秒级及微秒级部分，作为开始时间打印到屏幕上；随后执行累加函数；最后再次调用gettimeofday、timeval存储终止时间并打印，最终得到与开始时间的差打印到屏幕上即为总运行时间。最后退出并释放该模块。

# 实验结果及分析

![code](1.png)
![Makefile](2.png)
![output](3.png)
最终结果为累加求和总运行时间为55μs。

# 程序代码

## sum_time.c

```c
#include <linux/module.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");

#define NUM 100000
struct timeval tv;

static long sum(int num)
{
    int i;
    long total = 0;
    for (i = 1; i <= NUM; i++)
        total = total + i;
    printk("The sum of 1 to %d is: %ld\n", NUM, total);
    return total;
}

static int __init sum_init(void)
{
    int start;
    int start_u;
    int end;
    int end_u;
    long time_cost;
    long s;

    printk("Start sum_time module...\n");
    do_gettimeofday(&tv);
    start = (int)tv.tv_sec;
    start_u = (int)tv.tv_usec;
    printk("The start time is: %d s %d us \n", start, start_u);

    s = sum(NUM);

    do_gettimeofday(&tv);
    end = (int)tv.tv_sec;
    end_u = (int)tv.tv_usec;
    printk("The end time is: %d s %d us \n", end, end_u);
    time_cost = (end - start) * 1000000 + end_u - start_u;
    printk("The cost time of sum from 1 to %d is: %ld us \n", NUM, time_cost);
    return 0;
}

static void __exit sum_exit(void)
{
    printk("Exit sum_time module...\n");
}

module_init(sum_init);
module_exit(sum_exit);
```
## Makefile
```c
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
```