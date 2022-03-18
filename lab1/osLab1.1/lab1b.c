#include <linux/kernel.h>
#include <linux/module.h>
int init_module()
{
long iValue;
__asm__ __volatile__( "movq %%cr3,%0":"=r" ( iValue));printk ( "cr3:%ld\n" , iValue) ;
return 0;
}
void cleanup_module(void){
printk ( "uninstall getcr3 ! \n" );
}
