#include<linux/kernel.h>
#include<linux/module.h>
int init_module()
{
printk ( "Hello! This is a testing module! \n" ) ;
return 0;
}
void cleanup_module()
{
printk ( "Sorry! The testing module is unloading now! \n" ) ;
}

