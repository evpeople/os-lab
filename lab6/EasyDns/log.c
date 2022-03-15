#include "log.h"
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <string.h>

#define DBG_DEBUG 0x01
#define DBG_INFO 0x02
#define DBG_WARNING 0x04
#define DBG_ERROR 0x08
#define DBG_TEMP 0x10
static time_t epoch;

static int debug_mask = 0;
int port = 0;
uint serverAddress = 0;

static struct option intopts[] = {

    {"debug", required_argument, NULL, 'd'},
    {"server", no_argument, NULL, 's'},
    {0, 0, 0, 0},
};

#define OPT_SHORT "?ufind:p:b:l:t:s:"

void config(int argc, char **argv)
{
    char fname[1024];
    int i, opt;

    strcpy(fname, "");

    while ((opt = getopt_long(argc, argv, OPT_SHORT, intopts, NULL)) != -1)
    {
        switch (opt)
        {

        case 'd':
            debug_mask = atoi(optarg);
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case 's':
            serverAddress = inet_addr(optarg);
            break;
        default:
            printf("ERROR: Unsupported option\n");
        }
    }

    if (fname[0] == 0)
    {
        strcpy(fname, argv[0]);
        if (strcasecmp(fname + strlen(fname) - 4, ".exe") == 0)
            *(fname + strlen(fname) - 4) = 0;
        strcat(fname, ".log");
    }

    if (strcasecmp(fname, "nul") == 0)
        log_file = NULL;
    else if ((log_file = fopen(fname, "w")) == NULL)
        printf("WARNING: Failed to create log file \"%s\": %s\n", fname, strerror(errno));

    lprintf("Log file \"%s\", debug mask 0x%02x\n", fname, debug_mask);
}

void dbg_debug(char *ip, int num, char *fmt, ...)
{
    va_list arg_ptr;

    if (debug_mask & DBG_DEBUG)
    {
        printf("\033[34mDEBUG:\t");
        va_start(arg_ptr, fmt);
        __v_lprintf(fmt, arg_ptr);
        va_end(arg_ptr);
        if (num)
        {
            dbg_ip(ip, num);
        }
        printf("\033[0m");
    }
}

void dbg_info(char *fmt, ...)
{
    va_list arg_ptr;

    if (debug_mask & DBG_INFO)
    {
        printf("INFO: finished Func\t");
        va_start(arg_ptr, fmt);
        __v_lprintf(fmt, arg_ptr);
        va_end(arg_ptr);
    }
}

void dbg_warning(char *fmt, ...)
{
    va_list arg_ptr;

    if (debug_mask & DBG_WARNING)
    {
        printf("\033[35mWARNING:\t");
        va_start(arg_ptr, fmt);
        __v_lprintf(fmt, arg_ptr);
        va_end(arg_ptr);
        printf("\033[0m");
    }
}
void dbg_error(char *fmt, ...)
{
    va_list arg_ptr;

    if (debug_mask & DBG_ERROR)
    {
        printf("\033[31mERROR:\t");
        va_start(arg_ptr, fmt);
        __v_lprintf(fmt, arg_ptr);
        va_end(arg_ptr);
        printf("\033[0m");
    }
}
void dbg_temp(char *fmt, ...)
{
    va_list arg_ptr;

    if (debug_mask & DBG_TEMP)
    {
        printf("\033[36mTEMP:\t");
        va_start(arg_ptr, fmt);
        __v_lprintf(fmt, arg_ptr);
        va_end(arg_ptr);
        printf("\033[0m");
    }
}
void dbg_ip(unsigned char *temp, int n)
{
    printf("IP IS BEGIN \n");
    for (size_t i = 0; i < n; i++)
    {
        printf("%02X\t", *temp);
        temp++;
        if (i % 10 == 0 && i != 0)
        {
            printf("\n");
        }
    }
    printf("\nIP IS OVER  \n");
}

unsigned int get_ms(void)
{
    struct timeval tm;
    struct timezone tz;

    gettimeofday(&tm, &tz);

    return (unsigned int)(epoch ? (tm.tv_sec - epoch) * 1000 + tm.tv_usec / 1000 : 0);
}