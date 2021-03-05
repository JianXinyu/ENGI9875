/*
 * badcnt.c - An improperly synchronized counter program
 */
/* $begin badcnt */
/* WARNING: This code is buggy! */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


void *thread(void *vargp);  /* Thread routine prototype */

/* Global shared variable */
volatile long cnt = 0; /* Counter */

struct foo
{
    unsigned int f_count;
    pthread_mutex_t f_lock;
};

void increment(struct foo f)
{
    pthread_mutex_lock(&f.f_lock);
    f.f_count++;
    pthread_mutex_unlock(&f.f_lock);
}

struct foo * foo_create(void)
{
    struct foo *f = malloc(sizeof(struct foo *));
    f->f_count = 0;
    int rc = pthread_mutex_init(&f->f_lock, NULL);
    assert(rc == 0); // check init success
    return f;
}

int main(int argc, char **argv)
{
//    ^{ printf("Hello World\n"); }
    long niters;
    pthread_t tid1, tid2;

    /* Check input argument */
    if (argc != 2) {
        printf("usage: %s <niters>\n", argv[0]);
        exit(0);
    }
    niters = atoi(argv[1]);

    /* Create threads and wait for them to finish */
    pthread_create(&tid1, NULL, thread, &niters);
    pthread_create(&tid2, NULL, thread, &niters);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    /* Check result */
    if (cnt != (2 * niters))
        printf("BOOM! cnt=%ld\n", cnt);
    else
        printf("OK cnt=%ld\n", cnt);
    exit(0);
}

/* Thread routine */
void *thread(void *vargp)
{
    long i, niters = *((long *)vargp);

    for (i = 0; i < niters; i++) //line:conc:badcnt:beginloop
        cnt++;                   //line:conc:badcnt:endloop

    return NULL;
}
/* $end badcnt */
