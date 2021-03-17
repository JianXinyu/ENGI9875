/*
 * Copyright 2018 Jonathan Anderson
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rusage.h"


void
get_command_rusage(int argc, char * const argv[], char *buffer, size_t len)
{
	assert(argv[argc] == NULL);

	/*
	 * You will implement this in the "GUI" section of the lab.
	 */
}

void
get_rusage_string(char *buffer, size_t len)
{
	struct rusage *rptr = malloc(sizeof(struct rusage));
       	int rt;
	if((rt = getrusage(RUSAGE_SELF, rptr)) !=0){
		fprintf(stdout, "getrusage() error: %s\n", strerror(errno));
		exit(1);
	}
	snprintf(buffer, len, 	"user CPU time used		: %ld\nsystem CPU time used		: %ld\nmaximum resident set size	: %ld\nintegral shared memory size	: %ld\n"
				"integral unshared data size	: %ld\nintegral unshared stack size	: %ld\nminor faults			: %ld\nmajor faults			: %ld\n"
				"swaps				: %ld\nblock input operations		: %ld\nblock output operations		: %ld\nIPC messages sent		: %ld\n"
				"IPC messages received		: %ld\nsingals received		: %ld\nvoluntary context switches	: %ld\ninvoluntary context switches	: %ld\n",
            			rptr->ru_utime.tv_usec, rptr->ru_stime.tv_usec, rptr->ru_maxrss, rptr->ru_ixrss,
            		rptr->ru_idrss, rptr->ru_isrss, rptr->ru_minflt, rptr->ru_majflt, 
				rptr->ru_nswap, rptr->ru_inblock, rptr->ru_oublock, rptr->ru_msgsnd, 
			rptr->ru_msgrcv, rptr->ru_nsignals, rptr->ru_nvcsw, rptr->ru_nivcsw);
		
}
