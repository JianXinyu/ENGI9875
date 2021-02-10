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
#include <time.h>
#include <stdio.h>


extern void do_stuff(void);


int main(int argc, char *argv[])
{
	struct timespec begin, end;
	FILE *fptr;
	long long int time;
	fptr = fopen("time_syscall.txt","w");
	if(fptr == NULL)
	{
		printf("ERROR!");
		exit(1);
	}
	printf("Calling do_stuff()!\n");
	for(int i = 0; i < 1000; i++)
	{

		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&begin);
		do_stuff();
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&end);
		time =  end.tv_nsec - begin.tv_nsec;
		fprintf(fptr, "%lli\n", time);
	}
	printf("Back in main() again.\n");
	fclose(fptr);
	return 0;
}
