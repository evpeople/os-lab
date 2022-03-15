#include "net.h"
#include "hlist.h"
#include "log.h"
#include <time.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define DNS_MAX_PACKET 548
#define IP_LEN 40
#define ANS_LEN 1024
#define CACHE_LEN 299

#define NOT_FOUND 1
#define CANT_GIVE 2
#define GOT_IT 3
#define FROM_DNS 4
// #define SERVER_PORT 8889
#define MAX_EPOLL_SIZE 50

static int extraLen = 0;
extern int port;
extern uint serverAddress;

extern struct hashMap *hashMap;
extern struct hashMap *cacheMap;

static struct cache cacheForId[CACHE_LEN];

int lenOfQuery(char *rawmsg)
{
    int temp;
    temp = strlen(rawmsg) + 5;
    return temp;
    dbg_info("len of Query\n");
}

void makeDnsRR(char *buf, ulong *ip, int state)
{
    struct HEADER *header = (struct HEADER *)buf;
    struct ANS *rr;

    header->ancount = htons(1); //确定有多少条消息

    char *dn = buf + sizeof(struct HEADER);
    int lenght = lenOfQuery(dn);
    char *name = dn + lenght;
    unsigned short *_name = (unsigned short *)name;
    *_name = htons((unsigned short)0xC00C);

    rr = (struct ANS *)(name); //设置rr的类型
    rr->class = htons(1);
    rr->ttl = htons(0x1565);

    //数据区域
    char *temp = (char *)rr + 10;
    *temp = 0;

    if (state == CANT_GIVE) //屏蔽网站
    {
        dbg_warning("it is a bad net website");
        header->rcode = 5;
        // rr->rdlength = htons(25);
        rr->type = htons(16);
        *(temp + 1) = 24;
        *(temp + 2) = strlen("it is a bad net website");
        extraLen = strlen("it is a bad net website") - 3;
        char *data = (char *)rr + 13;
        strcpy(data, "it is a bad net website");
    }
    else //正常应答
    {
        rr->type = htons(1);
        *(temp + 1) = 4;
        ulong *data = (ulong *)((char *)rr + 12);
        *data = *ip;
    }

    dbg_info("makeDnsRR\n");
}

void makeDnsHead(char *rawmsg, ulong *ans, int stateCode)
{
    switch (stateCode)
    {
    case NOT_FOUND:

        break;
    case CANT_GIVE:
        ((struct HEADER *)rawmsg)->aa = 0;
        ((struct HEADER *)rawmsg)->qr = 1;
        ((struct HEADER *)rawmsg)->rcode = htons(5);
        ((struct HEADER *)rawmsg)->ra = 0;
        break;
    case GOT_IT:
        ((struct HEADER *)rawmsg)->aa = 0;
        ((struct HEADER *)rawmsg)->qr = 1;
        ((struct HEADER *)rawmsg)->rcode = 0;
        ((struct HEADER *)rawmsg)->ra = 0;
        break;
    case FROM_DNS:;
        uint16_t id = *(uint16_t *)rawmsg;
        ((struct HEADER *)rawmsg)->id = (cacheForId[id % CACHE_LEN].key);
        dbg_debug(rawmsg, 4, "from Dns webSite look ip \n");
        break;
    default:
        dbg_error("接收包的函数遇到严重的错误\n");
        break;
    }
    dbg_info("makeDnsHead\n");
}

void getAddress(char **rawMsg)
{
    char *p = *rawMsg;
    int temp = *p;
    while (*p != 0)
    {
        for (int i = temp; i >= 0; i--)
        {
            p++;
        }
        temp = *p;
        if (temp == 0)
        {
            break;
        }
        *p = '.';
    }
    (*rawMsg)++;
    dbg_info("getAddress\n");
}
void dealWithPacket(char *buf, const struct sockaddr *addr, int fd, int count)
{
    extraLen = 0;
    fflush(NULL);
    int stateCode = 0;

    char *rawmsg = malloc(sizeof(char) * ANS_LEN);
    memcpy(rawmsg, buf, ANS_LEN);
    char *domain = malloc(sizeof(char) * ANS_LEN);
    strcpy(domain, rawmsg + sizeof(struct HEADER));
    getAddress(&domain);
    dbg_debug(rawmsg, 0, "domain is %s \n", domain);
    if (isNotIpv4(&rawmsg) && isQuery(rawmsg))
    {
        sendToDns(rawmsg, addr, fd, count);
        stateCode = NOT_FOUND;
        dbg_info("is not ipv4\n");
    }
    else if (isQuery(rawmsg))
    {
        ulong ans;
        int ret = findHashMap(&cacheMap, domain, &ans);
        if (ret == 0)
        {
            dbg_temp("Not find in cacheMap\n");
            ret = findHashMap(&hashMap, domain, &ans);
            if (ret == 1)
            {
                dbg_warning("find in hashMap and store it\n");
                addHashMap(domain, ans, &cacheMap, -1);
            }
        }
        else
        {
            dbg_warning("find in cache\n");
        }

        free(domain - 1);

        if (ret == 0) //没有找到
        {
            dbg_debug(rawmsg, 0, "ask other DNS\n");
            stateCode = NOT_FOUND;
            sendToDns(rawmsg, addr, fd, count);
        }
        else if (ans == 0)
        {
            stateCode = CANT_GIVE;
        }
        else
        {
            stateCode = GOT_IT;
        }
        makeDnsHead(rawmsg, &ans, stateCode);
        makeDnsRR(rawmsg, &ans, stateCode);
    }
    else
    {
        stateCode = NOT_FOUND;
        uint16_t id = *(uint16_t *)rawmsg;
        if ((*(uint16_t *)(rawmsg + sizeof(struct HEADER) + lenOfQuery(rawmsg + sizeof(struct HEADER)) + 3)) == 1)
        {
            uint32_t ip = getIP(rawmsg);
            uint ttl = getTTl(rawmsg);
            dbg_temp("ip is %u ttl is %d\n", ip, ttl);
            addHashMap(domain, ip, &cacheMap, ttl);
        }

        free(domain - 1);
        makeDnsHead(rawmsg, NULL, FROM_DNS);
        struct sockaddr *address = malloc(sizeof(struct sockaddr));
        memcpy(address, cacheForId[id].value, sizeof(struct sockaddr));
        int len = sizeof(*address);
        sendto(fd, rawmsg, count, 0, address, len);
        dbg_debug(rawmsg, 4, "success got a ans from Dns and send it !!!!!!!\n");
        return;
    }
    //发送构造好的相应。
    if (stateCode != NOT_FOUND)
    {
        int len = sizeof(*addr);
        sendto(fd, rawmsg, count + sizeof(struct ANS) + extraLen, 0, addr, len);
        dbg_debug(rawmsg, 0, "find ip in cache or hash#####\n");
    }

    free(rawmsg);
    dbg_info("dealWithPacket\n");
}

void sendToDns(char *rawmsg, const struct sockaddr *addr, int fd, int count)
{
    struct sockaddr_in dnsAdd;

    addCacheMap(&rawmsg, addr);
    uint16_t id = *(uint16_t *)rawmsg;
    if (id)
    {
        dnsAdd.sin_family = AF_INET;
        serverAddress = serverAddress == 0 ? inet_addr("10.3.9.4") : serverAddress;
        dnsAdd.sin_addr.s_addr = serverAddress;
        dnsAdd.sin_port = htons(53);

        int len = sizeof(dnsAdd);

        int x = sendto(fd, rawmsg, count, 0, (const struct sockaddr *)&dnsAdd, len);
        id = 0;
    }
    dbg_info("sendToDns\n");
}
int isNotIpv4(char **rawmsg)
{
    char *p = *rawmsg + sizeof(struct HEADER);
    int temp = *p;
    while (*p != 0)
    {
        for (int i = temp; i >= 0; i--)
        {
            p++;
        }
        temp = *p;
        if (temp == 0)
        {
            p += 2;
            if (*p == 1)
            {
                return 0;
            }
            else
            {
                dbg_warning("is not a ipv4\n");
                return 1;
            }
            break;
        }
    }
    dbg_info("isNotIpv4\n");
}
uint32_t getIP(char *rawmsg)
{
    struct ANS *temp = (struct ANS *)(rawmsg + (sizeof(struct HEADER) + lenOfQuery(rawmsg + sizeof(struct HEADER))));
    char *y = (char *)&((*temp).rdlength);
    uint32_t *z = (uint32_t *)y;

    return *z;
    dbg_info("getIP\n");
}
uint getTTl(char *rawmsg)
{
    struct ANS *temp = (struct ANS *)(rawmsg + (sizeof(struct HEADER) + lenOfQuery(rawmsg + sizeof(struct HEADER))));
    char *x = (char *)&((*temp).ttl);
    x -= 2;
    uint32_t *z = (uint32_t *)x;
    uint32_t data = ntohl(*z);
    return data;
    dbg_info("getTTL\n");
}
bool isQuery(char *rawMsg)
{
    return !*(rawMsg + 3);
}
void initCache()
{
    for (size_t i = 0; i < CACHE_LEN; i++)
    {
        cacheForId[i].value == NULL;
    }
}
void addCacheMap(char **rawmsg, const struct sockaddr *addr)
{
    char *p = *rawmsg;
    uint16_t id = *(uint16_t *)(*rawmsg);
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    long idInCache = tv.tv_sec;
    cacheForId[idInCache % CACHE_LEN].key = id;
    if (!addr)
    {
        *(uint16_t *)(*rawmsg) = 0;
        return;
    }
    if (cacheForId[idInCache % CACHE_LEN].value != NULL)
    {
        free(cacheForId[idInCache % CACHE_LEN].value);
        cacheForId[idInCache % CACHE_LEN].value = NULL;
    }

    cacheForId[idInCache % CACHE_LEN].value = malloc(sizeof(struct sockaddr) + 4);
    memcpy(cacheForId[idInCache % CACHE_LEN].value, addr, sizeof(struct sockaddr));

    *(uint16_t *)(*rawmsg) = idInCache % CACHE_LEN;
    uint16_t id2 = *(uint16_t *)(*rawmsg);
    dbg_info("addCacheMap\n");
}
void runDns()
{
    int listenfd;
    listenfd = initSocket();
    if (listenfd == -1)
    {
        perror("can't create socket file");
        return;
    }
    if (setnonblocking(listenfd) < 0)
    {
        perror("setnonblock error");
    }
    int len;
    char buf[ANS_LEN];
    struct sockaddr_in clent_addr;
    len = sizeof(struct sockaddr_in);
    int epollfd = epoll_create(MAX_EPOLL_SIZE);
    struct epoll_event ev;
    struct epoll_event events[MAX_EPOLL_SIZE];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = listenfd;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
    while (1)
    {
        int nfds = epoll_wait(epollfd, events, 20, 500);
        for (size_t i = 0; i < nfds; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                int count = recvfrom(listenfd, buf, 1024, 0, (struct sockaddr *)&clent_addr, &len);
                char *rawmsg = malloc(sizeof(char) * ANS_LEN);
                memcpy(rawmsg, buf, ANS_LEN);
                setblocking(listenfd);
                dealWithPacket(rawmsg, (struct sockaddr *)&clent_addr, listenfd, count);
                setnonblocking(listenfd);
            }
        }
    }
}
int setnonblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
    {
        return -1;
    }
    return 0;
}

int setblocking(int sockfd)
{
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) & ~O_NONBLOCK) == -1)
    {
        return -1;
    }
    return 0;
}

int initSocket()
{
    int server_fd, ret;
    struct sockaddr_in ser_addr;

    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    port = port == 0 ? 8889 : port;
    ser_addr.sin_port = htons(port);

    ret = bind(server_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr));
    if (ret < 0)
    {
        printf("socket bind fail!\n");
        return -1;
    }
    dbg_info("initSocket\n");
    return server_fd;
}