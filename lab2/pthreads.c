/*
 * Copyright 2018 Jonathan Anderson
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <err.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

void *increment(void *vargp)
{
	long *countp = (long *) vargp;
	for (int i = 0; i < WORK_PER_JOB; i++)
	{
		(*countp)++;
	}
	return NULL;
}
//void increment(int *countp)
//{
//    for (int i = 0; i < WORK_PER_JOB; i++)
//    {
//        (*countp)++;
//    }
//}
int main(void)
{
	int counter = 0;
	struct timespec begin, end;
	pthread_t tid;
	// Set C locale settings to get niceties like thousands separators
	// for decimal numbers.
	setlocale(LC_NUMERIC, "");

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin) != 0)
	{
		err(-1, "Failed to get start time");
	}

	for (int i = 0; i < JOBS; i++)
	{
        pthread_create(&tid, NULL, increment, &counter);
	}

	if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end) != 0)
	{
		err(-1, "Failed to get end time");
	}

    for (int i = 0; i < JOBS; i++) {
        pthread_join(tid, NULL);
    }

    //		int rc;
//		if((rc=pthread_create(&tid, NULL, increment, &counter))!=0)
//		{
//			fprintf(stderr, "pthread_creat error: %s\n", strerror(rc));
//			exit(0);
//		}

//pthread_exit(NULL);

	long diff = end.tv_nsec - begin.tv_nsec;
	diff += (1000 * 1000 * 1000) * (end.tv_sec - begin.tv_sec);

	printf("Counted to %'d in %'ld ns: %f ns/iter\n",
	       JOBS * WORK_PER_JOB, diff, ((double) diff) / counter);

	return 0;
}


