#include "config.h"
#include "net.h"
#include <sys/epoll.h>
struct hashMap *hashMap;
struct hashMap *cacheMap;
int main(int argc, char **argv)
{
    config(argc, argv);
    initDNS(&hashMap);
    runDns();
}