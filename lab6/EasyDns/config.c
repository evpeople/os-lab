
#include "config.h"

extern struct hashMap *cacheMap;

void initDNS(struct hashMap **hashMap)
{
    createHasMap(hashMap);
    createHasMap(&cacheMap);
    hashMapInit(hashMap);
    dbg_info("initDNS\n");
}