#include <stdbool.h>
#include <stdint.h>
#include <sys/socket.h>
#include <arpa/inet.h>
struct HEADER
{
    unsigned id : 16;    /* query identification number */
    unsigned rd : 1;     /* recursion desired */
    unsigned tc : 1;     /* truncated message */
    unsigned aa : 1;     /* authoritive answer */
    unsigned opcode : 4; /* purpose of message */
    unsigned qr : 1;     /* response flag */
    unsigned rcode : 4;  /* response code */
    unsigned cd : 1;     /* checking disabled by resolver */
    unsigned ad : 1;     /* authentic data from named */
    unsigned z : 1;      /* unused bits, must be ZERO */
    unsigned ra : 1;     /* recursion available */
    uint16_t qdcount;    /* number of question entries */
    uint16_t ancount;    /* number of answer entries */
    uint16_t nscount;    /* number of authority entries */
    uint16_t arcount;    /* number of resource entries */
};
struct QUERY
{
    unsigned qtype : 16;  /* truncated message */
    unsigned qclass : 16; /* authoritive answer */
};
struct ANS
{
    uint16_t name;
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
};

typedef unsigned int uint;
typedef unsigned long ulong;
int lenOfQuery(char *rawmsg);
void makeDnsRR(char *buf, ulong *ip, int state);
void makeDnsHead(char *rawmsg, ulong *ans, int stateCode);
void getAddress(char **rawMsg);
uint32_t getIP(char *);
void dealWithPacket(char *buf, const struct sockaddr *addr, int fd, int count);
bool isQuery(char *rawMsg);
void addCacheMap(char **rawmsg, const struct sockaddr *addr);
int initSocket();
void runDns();
int setnonblocking(int sockfd);
int setblocking(int sockfd);
int isNotIpv4(char **rawmsg);
void sendToDns(char *rawmsg, const struct sockaddr *addr, int fd, int count);
uint getTTl(char *rawmsg);