#include "hlist.h"
#include "log.h"
#include "net.h"

#define LENOFKEY 40
int hashCode(char *key)
{
    ////UNIX系统使用的哈希
    char *k = key;
    unsigned long h = 0;
    while (*k)
    {
        h = (h << 4) + *k++;
        unsigned long g = h & 0xF0000000L;
        if (g)
        {
            h ^= g >> 24;
        }
        h &= ~g;
    }
    return h % MAPLENGTH;
}

void createHasMap(struct hashMap **hashMap)
{

    (*hashMap) = malloc(sizeof(struct hashMap));
    memset((*hashMap)->hlist, 0, sizeof((*hashMap)->hlist));
}
void addHashMap(char *key, uint32_t value, struct hashMap **hashMap, int ttl) //key 是 domin， value 是ip
{
    //存储基本数据
    struct domainMap *node;

    node = malloc(sizeof(struct domainMap));
    node->key = malloc(DOMAINLENTH);
    memcpy(node->key, key, DOMAINLENTH);
    node->value = value;
    node->TTL = ttl;

    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    node->lastCallTime = tv.tv_sec;

    node->hash.next = NULL;

    //构建哈希表
    int index = hashCode(key);
    if ((*hashMap)->hlist[index] == NULL)
    {
        (*hashMap)->hlist[index] = malloc(sizeof(struct hlistHead));
        (*hashMap)->hlist[index]->first = &(node->hash);
        node->hash.pprev = &((*hashMap)->hlist[index])->first;
    }
    else
    {
        node->hash.pprev = (*hashMap)->hlist[index]->first->pprev;
        node->hash.next = (*hashMap)->hlist[index]->first;
        (*hashMap)->hlist[index]->first->pprev = &(node->hash).next;
        *(node->hash.pprev) = &(node->hash);
    }
}
int notOverTime(struct domainMap *temp)
{
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    long time = tv.tv_sec;
    dbg_temp("TTL is %d \nover is %d \ntime is %d\n lastCallTime %d\n", temp->TTL, time - temp->lastCallTime, time, temp->lastCallTime);
    if (temp->TTL == -1 || (temp->TTL > (time - temp->lastCallTime)))
    {
        return 1;
    }
    dbg_warning("really over time!\n");
    delHashMapNode(&temp);
    return 0;
}
void hashMapInit(struct hashMap **hashMap)
{
    FILE *fp = NULL;
    char ip[IPLENGTH];
    char domain[DOMAINLENTH];
    fp = fopen("/home/wangzhe/DNS/DNS/dnsrelay.txt", "r");
    if (fp == NULL)
    {
        dbg_error("can't open file\n");
        exit(1);
    }
    while (!feof(fp))
    {
        if (!fscanf(fp, "%s", ip))
        {
            dbg_error("get ip wrong\n");
        }
        if (!fscanf(fp, "%s", domain))
        {
            dbg_error("get domain wrong\n");
        }
        addHashMap(domain, inet_addr(ip), hashMap, -1);
    }
    fclose(fp);
}

int findHashMap(struct hashMap **hashMap, char *key, ulong *value)
{
    bool find = false;
    int flag = 0;
    int index = hashCode(key);

    if ((*hashMap)->hlist[index] != NULL)
    {

        struct domainMap *temp = (struct domainMap *)((*hashMap)->hlist[index]->first - 2); //为内存偏移的起始地址
        ulong ttl = temp->TTL;
        if (ttl == -1)
        {
            flag = -1;
        }
        else
        {
            flag = 0;
        }

        //通过flag<=3 实现LRU
        while ((&(temp->hash) != NULL && flag == -1) || (&(temp->hash) != NULL && flag <= 3))
        {
            int a = notOverTime(temp);
            dbg_temp("over Time is %d \n", a);
            if (!strcasecmp(key, temp->key) && notOverTime(temp))
            {
                find = true;
                *value = temp->value;
                break;
                if (flag != -1)
                {
                    flag++;
                }
            }

            temp = (struct domainMap *)(temp->hash.next - 2);
        }
    }
    if (!find)
    {
    }
    return find;
}

void freeHashMap(struct hashMap **hashMap)
{

    free(*hashMap);
}
void delHashMapNode(struct domainMap **n)
{
    if (*n == NULL)
    {
        return;
    }

    struct hlistNode *next = (*n)->hash.next;
    struct hlistNode **pprev = (*n)->hash.pprev;
    // struct hlistNode *next = n->next;
    // struct hlistNode **pprev = n->pprev;
    *pprev = next;
    if (next)
        next->pprev = pprev;
}