#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <errno.h>
#define BUFFER_SIZE 128
#define SRC_FILE_NAME "src_file.txt"
#define DEST_FILE_NAME "dest_file.txt"
#define OFFSET 0
int main()
{
	int src_file,dest_file;
	unsigned char src_buff[BUFFER_SIZE];
	unsigned char dest_buff[BUFFER_SIZE];
	int real_read_len = 0;
	char str[BUFFER_SIZE] = "this is a testabout\nopen()\nclose()\nwrite()\nread()\nlseek()\nend of the file\n";
	src_file=open(SRC_FILE_NAME, O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	if(src_file<0)
	{
		printf("open file error!!!\n");
		exit(1);
	}
	write(src_file, str, sizeof(str));
	dest_file=open(DEST_FILE_NAME, O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH); 
	if(dest_file<0)
	{
		printf("open file error!!!\n");
		exit(1);
	}
	lseek(src_file, OFFSET, SEEK_SET); 
	while((real_read_len=read(src_file, src_buff, sizeof(src_buff)))>0)
	{
		printf("src_file:%s", src_buff);
 		write(dest_file,src_buff,real_read_len);
	}
	lseek(dest_file, OFFSET, SEEK_SET); 
	while((real_read_len=read(dest_file,dest_buff,sizeof(dest_buff)))>0); 
	printf("dest_file:%s", dest_buff);
	close(src_file);
	close(dest_file);
	return 0;
}