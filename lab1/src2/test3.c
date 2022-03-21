#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#define _SYSCALL_MYSETNICE_ 388
#define EFALUT 14

#include <string.h>
 
int main()
{
char * str="syscall_mycall";
int     res=syscall(_SYSCALL_MYSETNICE_,str,strlen(str)+1);
 printf("errno %d :\t\t%s\n",errno,strerror(errno));
return 0;
}