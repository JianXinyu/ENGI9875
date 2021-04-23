# MEMORY ALLOCATION

23:59 on 5 Feb 2021

Implement your own version of `malloc(3)`, `free(3)` and related functions.

Having discussed memory allocation mechanisms and strategies in [Lecture 5](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/5/), it’s time to build your own memory allocator! This assignment will help you:

- understand the challenges and trade-offs inherent in memory allocation,
- practice interacting with the OS kernel via system calls and
- practice implementing data structures in C.

## Directions

You can choose whatever memory allocation algorithm you like: best-fit, first-fit, buddy, something else, etc. I won’t require you to implement **all** of the `malloc(3)`-like functions, but I do want to see implementations of `malloc(3)`, `realloc(3)` and `free(3)`. In addition to functions that look like standard `libc` functions, you also need to implement some functions that can be used to control or query your allocation library; see the bottom of [the rtos-alloc.h header file](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/rtos-alloc.h).

In addition to implementing source code, you must **evaluate its performance**. I will expect you to plot the time taken to perform a variety of allocations of different sizes against the time taken by libc’s `malloc(3)` to perform the same allocations. You may wish to use `clock_gettime(2)` with the `CLOCK_PROCESS_CPUTIME_ID` argument (or see [this helpful reference](http://www.cs.utexas.edu/~pingali/CS377P/2017sp/lectures/measurements.pdf)). The choice of what range of values should go on the X axis of the plot is left to you to investigate.

## Materials

I’m providing you with a few header and source files to get started with. You only need the first header file ([rtos-alloc.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/rtos-alloc.h)), but you may find the others helpful as you work through the problem.

- [rtos-alloc.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/rtos-alloc.h)

  This is the header file that declares the functions you need to implement.

- [test.cpp](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/test.cpp)

  This is some test code that I’ve written for these functions. I don’t claim that it’s exhaustive (yet) and I reserve the right to add more tests between now and the due date, but this should get you going in the right direction when thinking about tests.

- [passthrough.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/passthrough.c)

- [passthrough-internal.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/passthrough-internal.h)

- [passthrough-internal.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/2/passthrough-internal.c)

  This is an implementation of the required functions that, instead of doing the actual work of allocation itself, simply passes calls through to `malloc(3)`, `free(3)`, etc. It’s actually more complicated than that, as it keeps track of outstanding allocations in order to answer queries such as `rtos_alloc_size()`, but it’s still not a real solution to the problem I asked you to address. As such, feel free to inspect it, use it in your test code and perhaps even pick up a few C idioms.

## Deliverables

To complete this assignment you must submit both source code and your plot(s) to Gradescope. Use whatever vector or raster image format is convenient for you.

# Autograder Results

AUTOGRADER SCORE 8.0 / 8.0

Autograder Output

```
Submission dir: 'submission'
Student submitted 1 file(s):
  submission/rtos-alloc.c: C source, ASCII text

1 source file(s):
  submission/rtos-alloc.c: C source, ASCII text

Compiling 'submission/rtos-alloc.c'...
clang -c -Wall -fPIC -I . -I /usr/local/include submission/rtos-alloc.c -o submission/rtos-alloc.c.o

Compiling tests...
clang++ -c -std=c++17 -Wall -fPIC -I . -I /usr/local/include -Wall tests.cpp -o tests.o

Linking tests with submission...
clang++ tests.o submission/rtos-alloc.c.o -lgrading -L /usr/local/lib -o tester

Running submission... OK
```

basic allocation

```
Test description:
 - allocate a pointer with rtos_malloc()
 - check that ptr is not NULL

--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

large allocation

```
Test description:
 - allocate 100 MiB with rtos_malloc()
 - check that ptr is not NULL

--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

use a large allocation

```
Test description:
 - allocate 100 M size_t array with rtos_malloc()
 - check that ptr is not NULL - write data to the allocation

--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

valid allocation

```
Test description:
 - allocate a pointer with rtos_malloc(8)
 - check that rtos_is_valid(ptr) returns true
 - check that rtos_alloc_size(ptr) returns no less than 8

--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

NULL is not a valid allocation

```
Test description:
 - check that rtos_is_valid(NULL) returns false

--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

free()

```
Test description:
 - allocate a pointer; check that it's valid
 - free the pointer
 - check that the pointer is no longer valid


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

free(NULL)

```
Test description:
 - free the NULL pointer (should cause no action)
 - check that NULL remains invalid


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```

multiple allocations

```
Test description:
 - allocate several objects of different sizes
 - check that each allocation is no smaller than requested
 - check that the total allocation size is the sum of parts


--------------------------------------------------------------------------------
Console output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Error output:
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
Result: passed
```