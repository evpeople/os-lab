#include <stdio.h>
//用户空间的标准输入输出头文件
void GetCr3( )
{
long iValue;
__asm__ __volatile__( "movq %%cr3,%0": "= r" ( iValue) ) ;
printf ( "the value in cr3 is: %d", iValue);
}

int main( )
{
GetCr3 ( );
return 0;
}
