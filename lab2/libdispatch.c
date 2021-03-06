#include <err.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dispatch/dispatch.h>

void *increment(void *vargp);

// dispatch_semaphore_t semaphore;

int main(int argc, char *argv[])
{
    int counter = 0;
	struct timespec begin, end;
	dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
	dispatch_group_t group = dispatch_group_create();
	// semaphore = dispatch_semaphore_create(1);

	setlocale(LC_NUMERIC, "");

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) != 0)
	{
		err(-1, "Failed to get start time");
	}

	for (int i = 0; i < JOBS; i++)
	{
		dispatch_group_async_f(group, queue, &counter, increment);
	}
	dispatch_group_wait(group, DISPATCH_TIME_FOREVER);

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) != 0)
	{
		err(-1, "Failed to get end time");
	}

	long diff = end.tv_nsec - begin.tv_nsec;
	diff += (1000 * 1000 * 1000) * (end.tv_sec - begin.tv_sec);

	printf("Counted to %'d in %'ld ns: %f ns/iter\n",
	       counter, diff, ((double) diff) / counter);

	return 0;
}

void *increment(void *vargp)
{
	long *countp = (long *) vargp;
	for (int i = 0; i < WORK_PER_JOB; i++)
	{
		// dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
		(*countp)++;
		// dispatch_semaphore_signal(semaphore);
	}
    return NULL;
}
