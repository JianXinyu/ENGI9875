#include "rtos-alloc.h"

#include <libgrading.h>

#include<stdio.h> /*forprintf */
#include<stdint.h> /*foruint64*/
#include<time.h> /*forclock_gettime */

#include <fstream> // store the data into a file

using namespace grading;


const TestSuite tests = {
	{
		"basic allocation",
		" - allocate a pointer with rtos_malloc()\n"
		" - check that ptr is not NULL"
		,
		[]()
		{
			void *p = rtos_malloc(8);
			CheckNonNull(p, "allocated pointer should not be NULL");
		}
	},

	{
		"valid allocation",
		" - allocate a pointer with rtos_malloc(8)\n"
		" - check that rtos_is_valid(ptr) returns true\n"
		" - check that rtos_alloc_size(ptr) returns no less than 8"
		,
		[]()
		{
			void *p = rtos_malloc(8);
			Check(rtos_is_valid(p),
			      "allocated pointer should be valid");
			Check(rtos_alloc_size(p) >= 8,
			      "allocation should be no smaller than requested");
		}
	},

	{
		"NULL is not a valid allocation",
		" - check that rtos_is_valid(NULL) returns false"
		,
		[]()
		{
			Check(not rtos_is_valid(NULL),
			      "NULL is not a valid allocation");
		}
	},

	{
		"free()",
		" - allocate a pointer; check that it's valid\n"
		" - free the pointer\n"
		" - check that the pointer is no longer valid\n"
		,
		[]()
		{
			void *p = rtos_malloc(16);
			Check(rtos_is_valid(p),
			      "allocated pointer should be valid");

			rtos_free(p);
			Check(not rtos_is_valid(p),
			      "pointer should not be valid after freeing it");
		}
	},

	{
		"free(NULL)",
		" - free the NULL pointer (should cause no action)\n"
		" - check that NULL remains invalid\n"
		,
		[]()
		{
			rtos_free(NULL);
			Check(not rtos_is_valid(NULL),
			      "NULL is not a valid allocation");
		}
	},

	{
		"multiple allocations",
		" - allocate several objects of different sizes\n"
		" - check that each allocation is no smaller than requested\n"
		" - check that the total allocation size is the sum of parts\n"
		,
		[]()
		{
			size_t total = rtos_total_allocated();
			std::vector<void*> pointers;

			for (size_t s : { 1, 2, 17, 42, 1049, 50 })
			{
				void *p = rtos_malloc(s);
				size_t size = rtos_alloc_size(p);
				Check(size >= s,
				      "allocation size must be >= requested")
					<< "requested " << s
					<< ", got " << size;

				pointers.push_back(p);
				total += size;
			}

			CheckInt(total, rtos_total_allocated())
				<< "invalid total allocation size";

			for (void *p : pointers)
			{
				rtos_free(p);
			}
		}
	},
};

/**
 * timer for rtos_malloc() and malloc()
 * @param   fct(size_t)   rtos_malloc() or malloc()
 * @param   size    size
 * @return  running time [nanosecond]
 */
size_t Timer(void*	fct(size_t),size_t size)
{
    size_t execTime;/*timeinnanoseconds*/
    struct timespec tick, tock;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&tick);
    /*do stuff*/
    fct(size);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&tock);
    execTime=1000000000*(tock.tv_sec - tick.tv_sec)+tock.tv_nsec - tick.tv_nsec;
    printf("%llu \n",(long long unsigned int)execTime);
    return execTime;
}

int main(int argc, char *argv[])
{
//	const TestSuite::Statistics stats = tests.Run(argc, argv);
//	std::cout << "Score: " << (2 + 5 * stats.score) << "/7" << std::endl;


    //Open file in write mode
    std::ofstream outfile;
    outfile.open("time.txt");
    //Write the header
    outfile << "Size malloc() rtos_malloc()\n";

    for(size_t size = 10e5; size < 10e8; size+=10e5)
    {
        outfile << size << ' ' << Timer(malloc, size) << ' ' << Timer(rtos_malloc, size) << '\n';
    }

    outfile.close();

	return 0;
}


//void* rtos_realloc(void *block, size_t size)
//{
//    header_t *header;
//    void *ret;
//    if (!block || !size)
//        return rtos_malloc(size);
//    header = (header_t*)block - 1;
//    if (header->s.size >= size)
//        return block;
//    ret = rtos_malloc(size);
//    if (ret) {
//        /* Relocate contents to the new bigger block */
//        memcpy(ret, block, header->s.size);
//        /* Free the old memory block */
//        rtos_free(block);
//    }
//    return ret;
//}
