#include "rtos-alloc.h"

#include <unistd.h>
#include <string.h>
#include <assert.h>

//! reference:
//1. the C programming language 2ed edition, section 8.7
//2. https://arjunsreedharan.org/post/148675821737/write-a-simple-memory-allocator

typedef char ALIGN[16]; // for alignment
union header{           // block header
    struct {
        size_t size;    // block size
        unsigned is_free;
        union header *next; // next block if on free list
    } s;
    ALIGN stub; // force alignment of blocks
};
typedef union header header_t;

header_t *head = NULL, *tail = NULL; // first and last block to keep track of the list

void* rtos_malloc(size_t size)
{
    size_t total_size;
    header_t *header;
    void *block;

    // if size = 0, no need to allocate memory
    if(!size)
        return NULL;

    // check the allocated blocks to see if there's a free block that
    // can accommodate requested size by traversing (first-fit)
    header = head;
    while(header)
    {
        if(header->s.is_free && header->s.size >= size)
            break;
        header = header->s.next;
    }

    // if there is such a free block
    // mark it as unfree, and return the actual memory address
    if (header) {
        header->s.is_free = 0;
        return (void*)(header + 1);
    }

    // if there is no such a free block, allocate new block
    // new block size = header size + actual block size
    total_size = sizeof(header_t) + size;
    block = sbrk(total_size);

    // if allocation fail, return NULL
    if (block == (void*) -1) {
        return NULL;
    }
    // if allocation success, let header = new allocated block
    header = block;
    header->s.size = size;
    header->s.is_free = 0;
    header->s.next = NULL;

    // if there is no head yet, make header as head
    if(!head)
        head = header;
    // if there is already a tail, make header as new tail
    if(tail)
        tail->s.next = header;
    tail = header;

    // return the actual memory address
    return (void*)(header + 1);
}


void rtos_free(void *block)
{
    // if block is NULL
    if (!block)
        return;

    header_t *header, *tmp;
    void *brk; // end of the heap memory address

    header = (header_t*)block - 1; // point to block header
    brk = sbrk(0); // get current brk

    // if the block to be freed is the last one in the block list
    if ((char*)block + header->s.size == brk) {
        // shrink heap size
        // if there is only one block, simply delete it
        if (head == tail) {
            head = tail = NULL;
        }
        // else, move tail forward one block
        else {
            tmp = head;
            // iterate all blocks to find the block before the tail, make it tail
            while (tmp) {
                if(tmp->s.next == tail) {
                    tail = tmp;
                    tail->s.next = NULL;
                }
                tmp = tmp->s.next;
            }
        }

        // release the memory to OS
        sbrk(0 - header->s.size - sizeof(header_t));
        return;
    }

    // if not, simply mark it as free
    header->s.is_free = 1;

}

header_t* alloc_find(void *p)
{
    for (header_t *hp = head; hp != NULL; hp = hp->s.next){
        if (hp == (header_t*) p - 1){
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

    for (header_t *hp = head; hp != NULL; hp = hp->s.next){
        total += hp->s.size;
    }

    return total;
}
