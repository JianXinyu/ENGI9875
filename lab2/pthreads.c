#include <err.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

void *increment(void *vargp);
// volatile pthread_mutex_t plock;
int main(void)
{
	int counter = 0;
	struct timespec begin, end;
	pthread_t **tids = malloc(sizeof(pthread_t *)*JOBS);
	// int rc1 = pthread_mutex_init(&plock, NULL);
	// if(rc1 != 0)
	// {
	// 	fprintf(stderr, "pthread_init error: %s\n", strerror(rc1));
	// 	exit(0);
	// }
    
	for(int i = 0; i < JOBS; i++){
		tids[i] = malloc(sizeof(pthread_t));		
	}

	setlocale(LC_NUMERIC, "");

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) != 0)
	{
		err(-1, "Failed to get start time");
	}

	for(int i = 0; i < JOBS; i++){
              int rc2 = pthread_create(tids[i], NULL, increment, &counter);
              if(rc2!=0)
              {
                      fprintf(stderr, "pthread_creat error: %s\n", strerror(rc2));
                      exit(0);
              }
	}
	for (int i = 0; i < JOBS; i++) {
       		 pthread_join(*tids[i], NULL);
   	}

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) != 0)
	{
		err(-1, "Failed to get end time");
	}

	long diff = end.tv_nsec - begin.tv_nsec;
	diff += (1000 * 1000 * 1000) * (end.tv_sec - begin.tv_sec);

	printf("Counted to %'d in %'ld ns: %f ns/iter\n",
	       counter, diff, ((double) diff) / counter);

	free(tids);
	return 0;
}

void *increment(void *vargp)
{
	long *countp = (long *) vargp;
	for (int i = 0; i < WORK_PER_JOB; i++)
	{
		// pthread_mutex_lock(&plock);
		(*countp)++;
		// pthread_mutex_unlock(&plock);
	}
	return NULL;
}
