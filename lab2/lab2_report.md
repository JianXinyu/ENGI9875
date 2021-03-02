# Threading

## Purpose and outcomes

The goal of this lab is to explore two models for Unix userspace multithreading: POSIX threads and libdispatch. By the end of this lab you should be able to:

1. use POSIX threading primitives to write multithreaded programs,
2. explore the kernel threading behaviour of such a program and
3. compare the performance of alternative threading models for different workloads.

## Preparation

### Prelab

Answer all of the following questions. Provide evidence for your claims.

1. What is the underlying type of a `pthread_t`? What is its meaning? When is a `pthread_t` unique? When may it be reused?

   On my system - Ubuntu 18.04.5 LTS, `phthread_t` is defined as follows:

   ```C
   /* Thread identifiers.  The structure of the attribute type is not
      exposed on purpose.  */
   typedef unsigned long int pthread_t;
   ```

   A thread ID is represented by the `pthread_t` data type. But the implementations can use different structures, which is system independent[^1]. 

   > - Linux 3.2.0 uses an unsigned long integer for the pthread_t data type. 
   >
   > - Solaris 10 represents the pthread_t data type as an unsigned integer. 
   > - FreeBSD 8.0 and Mac OS X 10.6.8 use a pointer to the pthread structure for the pthread_t data type.

   Unlike the process ID, which is unique in the system, the thread ID has significance only within the context of the process to which it belongs[^1]. According to the `man pthread_self`:

   > Thread IDs are guaranteed to **be unique only within a process**.  A thread ID may be reused after a terminated thread has been joined, or a detached thread has terminated.

   

2. What is a *thread-safe* function? Provide an example implementation of a simple C function that is **not** thread-safe and an improved version that **is** thread-safe.

   > A function is said to be thread-safe if and only if it will always produce correct results when called repeatedly from multiple concurrent threads.  [^2]

   A class of thread-unsafe functions are functions that do not protect shared variables. Here is an example:

   ```c
   volatile long cnt = 0;
   void *thread(void *vargp){
       long i, niters = *((long *)vargp);
       for(i = 0; i < niters; i++)
           cnt++;
       return NULL;
   }
   ```

   the result of `cnt` is unpredictable. 

   We can use semaphores to implement mutual exclusion:

   ```c
   volatile long cnt = 0;
   sem_t mutex;
   void *thread(void *vargp){
       long i, niters = *((long *)vargp);
       for(i = 0; i < niters; i++){
           P(&mutex);
        	cnt++;   
           V(&mutex);
       }        
       return NULL;
   }
   ```

   Note that we should initialize `mutex` to unity in the main routine:

   ```c
   sem_init(&mutex, 0, 1);
   ```

   

3. Given the following C structure:

   ```c
   struct foo
   {
           unsigned int    f_count;
           pthread_mutex_t   f_lock;
   };
   ```

   1. Implement a function `void increment(struct foo *)`, which increments the `f_count` field in a thread-safe manner.

   2. Implement the function `struct foo * foo_create(void)`.

      ```c
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
          assert(rc == 0); // check if init successes
          return f;
      }
      ```

      

4. Read the first three sections of [Apple’s `libdispatch` tutorial](https://apple.github.io/swift-corelibs-libdispatch/tutorial) (up to, and including, "Defining work items: Functions and Blocks"). How do the C blocks described in this document compare to C++11 lambda functions?

   From the definition, they perform similar abilities. 

   > a block as a snippet of code that can be passed around and executed like a function where it is required.

   > a lambda expression, or lambda, is a convenient way of defining an anonymous function object (a closure) right at the location where it is invoked or passed as an argument to a function. Typically lambdas are used to encapsulate a few lines of code that are passed to algorithms or asynchronous methods.[^3]

   From the syntax, they look similar. Both of them

   - have arguments
   - can specify return types
     - block defines return types after the caret (\^)[^4]
     - lambda defines return types using trailing-return-type Optional
   - can be assigned to variables, and then invoked using the variable name

   ```C
   // define a block
   void (^speak)() = ^(char *x){ printf("%s %s\n", greeting, x); };
   // defien a lambda
   auto f1 = [](int x, int y) { return x + y; };
   ```

   

## Procedure

### Serial execution

Download the C source file [serial.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/2/serial.c). What does it do? Using the provided [Makefile](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/2/Makefile), compile the source file into an executable program and run it. How long is required to execute each unit of work?

Using the `awk(1)` program, filter the output of your program to keep only the time per work unit, outputting the result into a file:

```console
$ ./serial | awk '{ print $7 }' | tee -a initial-serial-times.dat
```

Use the shell to run this command 999 more times. Using `head(1)` and `ministat(1)`, report the average and standard deviation for the first 3, 5, 10, 100 and 1000 runs. Comment on the significance of the number of measurements when measuring physical phenomena.

Explore the behaviour of this program by measuring the average per-unit-of-work execution time over varying values of `JOBS` and `WORK_PER_JOB`. For example, to delete (`clean`) the current version of the `serial` program and compile a new version with `JOBS` set to 100 and `WORK_PER_JOB` set to 100, you can run the following command:

```console
$ make JOBS=100 WORK_PER_JOB=100 clean serial
clang-5.0 -D JOBS=1000 -D WORK_PER_JOB=10 -Weverything -Wno-unused-parameter serial.c   -o serial
```

How does the execution time vary with respect to `JOBS` and `WORK_PER_JOB`?

### POSIX threads

Presenting complex data

Your computer should have [R](https://www.r-project.org/), a statistical analysis program, installed (you can start an R GUI session by running `R --gui=Tk`). R is not the only way to present statistical information, but it can draw boxplots, violin plots, 3D surface plots and contour plots. It’s a useful tool to learn.

Copy `serial.c` to a new file called `pthreads.c`; add it to the `Makefile`. Modify this new file to execute the work in parallel using `JOBS` threads (note: do **not** use your thread-safe `increment` implementation from the prelab — that will come in later). As you did in the previous section, measure the average time to complete each unit of work for varying values of `JOBS` and `WORK_PER_JOB`. Present these results using appropriate graphical techniques. Also plot *throughput speedup* vs number of parallel threads.

### libdispatch

Copy `serial.c` to a new file called `libdispatch.c`; add it to the `Makefile` as well. Modify this new file to execute the work using `libdispatch` with `JOBS` asynchronous jobs. As you did in the previous section, measure the average time to complete each unit of work for varying values of `JOBS` and `WORK_PER_JOB`. Present these results using appropriate graphical techniques.

### Race conditions

Modify all three of your programs to print the actual value of `counter` rather than `JOBS * WORK_PER_JOB`. Explore how this value changes for varying values of `JOBS` and `WORK_PER_JOB` for both POSIX threads and `libdispatch`. Finally, modify your parallel programs to ensure that the correct value is counted. Plot throughput speed vs number of parallel threads in the POSIX case. Discuss your observations.



[^1]: https://www.informit.com/articles/article.aspx?p=2085690&seqNum=3
[^2]: CSAPP 3rd Section 12.7.1 
[^3]: https://docs.microsoft.com/en-us/cpp/cpp/lambda-expressions-in-cpp?view=msvc-160
[^ 4]: https://livebook.manning.com/book/objective-c-fundamentals/chapter-13/22 