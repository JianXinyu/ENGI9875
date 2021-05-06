#include "csapp.h"
#include "cache.h"



void cache_init(web_cache *cache, int n)
{
    cache->n = n;
    cache->size_all = cache->read_cnt = cache->rear = cache->front = cache->objn_cnt = 0;
    cache->obj = Calloc(n, sizeof(web_obj));
    Sem_init(&cache->mutex, 0, 1);
    Sem_init(&cache->write, 0, 1);
}

char* cache_find(web_cache *cache, char* dst)
{
    P(&cache->mutex);
    cache->read_cnt++;
    if (cache->read_cnt == 1)
        P(&cache->write);
    V(&cache->mutex);
    
    int i, n = cache->n;
    int l = (cache->front+1) % n, len = cache->objn_cnt;
    char *ans;
    for (i = 0; i < len; i++)
    {
        web_obj *obj = cache->obj + l;
        if (!strcmp(obj->dst, dst)) {
            int size = obj->size;
            ans = Malloc(size);
            strcpy(ans, obj->cache);
            break;
        }
        l = (l + 1) % n;
    }
    
    P(&cache->mutex);
    cache->read_cnt--;
    if (cache->read_cnt == 0)
        V(&cache->write);
    V(&cache->mutex);
    if (i == len) return NULL;
    else return ans;
}

void cache_put(web_cache *wcache, char* dst, char* cache)
{
    P(&wcache->write);
    
    int cnt = wcache->objn_cnt, n = wcache->n;
    if (cnt == n) {
        int pos = (wcache->front+1) % n;
        cache_remove(wcache, pos);
    }
    
    int pos = (++wcache->rear) % n, size = strlen(cache);
    web_obj *obj = wcache->obj + pos;
    strcpy(obj->dst, dst);
    strcpy(obj->cache, cache);
    obj->size = size;
    
    wcache->size_all += size;
    wcache->objn_cnt++;
    while(wcache->size_all > MAX_CACHE_SIZE) {
        int pos = (wcache->front+1) % n;
        cache_remove(wcache, pos);
    }
    
    V(&wcache->write);
}

void cache_remove(web_cache *cache, int pos)
{
    web_obj *obj = cache->obj + pos;
    cache->size_all -= obj->size;
    cache->front = (cache->front+1) % cache->n;
    cache->objn_cnt--;
}