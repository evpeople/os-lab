# include<stdio.h>
/*C程序必要的头文件*/
/*types.h头文件中定义了基本的数据类型。所有的类型定义为适当的数学类型长度。另外,size_
t是尤符号整数类型, off_t是扩展的符号整数类型, pid_t是符号整数类型。*/
#include <sys/types.h>
/*头文件stat.h说明了函数 stat()返回的数据及其结构类型,以及一些属性操作测试宏﹑函数原
型。*/
#include <sys/stat.h>
#include <stdlib.h>
/*exit()函数原型定义*/
#include <fcntl.h>
/*与文件操作相关*/
int main()
{
int i, testgetdev;
char buf[10];
/*字符数组，用于获取从read()写入的数据*/
testgetdev = open( "/dev/labc",O_RDONLY);/*打开前面所注册的设备文件*/
/*异常处理*/
if(testgetdev == -1) {
	printf ( "I Can't open the file! \n" );
	exit(0);
	}
/*调用read()函数,read()函数将10个字符7写入用户的缓冲区 buffer 数组*/
read(testgetdev,buf,10) ;
/*输出数组buffer */
for(i = 0; i<10; i++)
	printf( "NO. %d character is : %d\n",i+ 1, buf[ i]);
close(testgetdev) ;
/*事实上是调用release()函数关闭模块*/
return 0;
}

