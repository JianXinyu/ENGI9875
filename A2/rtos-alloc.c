#include "rtos-alloc.h"

#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>

typedef char ALIGN[16];

union header{
    struct {
        size_t size;
        unsigned is_free;
        union header *next;
    } s;
    ALIGN stub; // align the end of the header memory address
};
typedef union header header_t;

header_t *head = NULL, *tail = NULL; // keep track of the list
pthread_mutex_t global_malloc_lock; // prevent several threads from concurrently accessing memory

header_t *get_free_block(size_t size)
{
    header_t *curr = head;
    while(curr) {
        /* see if there's a free block that can accommodate requested size */
        if (curr->s.is_free && curr->s.size >= size)
            return curr;
        curr = curr->s.next;
    }
    return NULL;
}

void* rtos_malloc(size_t size)
{
    size_t total_size;
    header_t *header;
    void *block;

    if(!size)
        return NULL;

    pthread_mutex_lock(&global_malloc_lock);
    header = get_free_block(size);

    if (header) {
        header->s.is_free = 0;
        pthread_mutex_unlock(&global_malloc_lock);
        return (void*)(header + 1);
    }

    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);

    if (block == (void*) -1) {
        pthread_mutex_unlock(&global_malloc_lock);
        return NULL;
    }
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;

    if(!head)
        head = header;
    if(tail)
        tail->s.next = header;
    tail = header;

    pthread_mutex_unlock(&global_malloc_lock);

    return (void*)(header + 1);
}


void* rtos_realloc(void *block, size_t size)
{
    header_t *header;
    void *ret;
    if (!block || !size)
        return rtos_malloc(size);
    header = (header_t*)block - 1;
    if (header->s.size >= size)
        return block;
    ret = rtos_malloc(size);
    if (ret) {
        /* Relocate contents to the new bigger block */
        memcpy(ret, block, header->s.size);
        /* Free the old memory block */
        rtos_free(block);
    }
    return ret;
}

void rtos_free(void *block)
{
    header_t *header, *tmp;
    /* program break is the end of the process's data segment */
    void *programbreak;

    if (!block)
        return;
    pthread_mutex_lock(&global_malloc_lock);
    header = (header_t*)block - 1;
    /* sbrk(0) gives the current program break address */
    programbreak = sbrk(0);

    /*
       Check if the block to be freed is the last one in the
       linked list. If it is, then we could shrink the size of the
       heap and release memory to OS. Else, we will keep the block
       but mark it as free.
     */
    if ((char*)block + header->s.size == programbreak) {
        if (head == tail) {
            head = tail = NULL;
        } else {
            tmp = head;
            while (tmp) {
                if(tmp->s.next == tail) {
                    tmp->s.next = NULL;
                    tail = tmp;
                }
                tmp = tmp->s.next;
            }
        }
        /*
           sbrk() with a negative argument decrements the program break.
           So memory is released by the program to OS.
        */
        sbrk(0 - header->s.size - sizeof(header_t));
        /* Note: This lock does not really assure thread
           safety, because sbrk() itself is not really
           thread safe. Suppose there occurs a foregin sbrk(N)
           after we find the program break and before we decrement
           it, then we end up realeasing the memory obtained by
           the foreign sbrk().
        */
        pthread_mutex_unlock(&global_malloc_lock);
        return;
    }
    header->s.is_free = 1;
    pthread_mutex_unlock(&global_malloc_lock);
}

header_t* alloc_find(void *p)
{
    for (header_t *hp = head; hp != NULL; hp = hp->s.next)
    {
//        assert(hp != NULL);
        if (hp == (header_t*) p - 1)
        {
            return hp;
        }
    }

    return NULL;
}

size_t	rtos_alloc_size(void *ptr)
{
    header_t *ap = alloc_find(ptr);
    assert(ap != NULL);

    return ap->s.size;
}

bool rtos_is_valid(void *ptr)
{
    return alloc_find(ptr);
}

size_t rtos_total_allocated()
{
    size_t total = 0;

    for (header_t *hp = head; hp != NULL; hp = hp->s.next)
    {
        total += hp->s.size;
    }

    return total;
}
