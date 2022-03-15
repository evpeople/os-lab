
#include <stdarg.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define DBG_DEBUG 0x01
#define DBG_INFO 0x02
#define DBG_WARNING 0x04
#define DBG_ERROR 0x08
#define DBG_TEMP 0x10

extern FILE *log_file;
extern unsigned int get_ms(void);

int lprintf(const char *format, ...);
int __v_lprintf(const char *format, va_list arg_ptr);

void dbg_temp(char *fmt, ...);
void dbg_error(char *fmt, ...);
void dbg_warning(char *fmt, ...);
void dbg_info(char *fmt, ...);
void dbg_debug(char *ip, int num, char *fmt, ...);
void dbg_ip(unsigned char *, int);
void config(int argc, char **argv);
