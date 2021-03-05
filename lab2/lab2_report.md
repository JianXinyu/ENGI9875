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

```bash
xy@xy-vm ~/D/E/lab2> ./serial
Counted to 100 in 1,928 ns: 19.280000 ns/iter
```

Using the `awk(1)` program, filter the output of your program to keep only the time per work unit, outputting the result into a file:

```console
$ ./serial | awk '{ print $7 }' | tee -a initial-serial-times.dat
```

Use the shell to run this command 999 more times. Using `head(1)` and `ministat(1)`, report the average and standard deviation for the first 3, 5, 10, 100 and 1000 runs. Comment on the significance of the number of measurements when measuring physical phenomena.

```bash
$ for i in $(seq 1000); do ./serial | awk '{ print $7 }' | tee -a initial_serial_times.dat; done

$ for i in {3,5,10,100,1000}; do head -$i initial_serial_times.dat | ministat -A -s; done
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x   3         20.01         21.02         20.82     20.616667    0.53482084
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x   5         19.83          21.4         20.82        20.616    0.67166212
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x  10         19.58         30.93          21.4        22.578     3.6707426
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x 100         17.19         431.7         24.36       33.5844     42.934708
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x 1000         14.97       1758.24         23.81      37.61954     89.585545
```

With the number of measurements increase, we are highly likely to have bad data, e.g., we have max data point 1758 with N=1000, which will dramatically influence the Average and Standard Deviation. Since the number of bad points are small, they don't influence Median very much. 



Explore the behaviour of this program by measuring the average per-unit-of-work execution time over varying values of `JOBS` and `WORK_PER_JOB`. For example, to delete (`clean`) the current version of the `serial` program and compile a new version with `JOBS` set to 100 and `WORK_PER_JOB` set to 100, you can run the following command:

```shell
$ make JOBS=100 WORK_PER_JOB=100 clean serial
```

How does the execution time vary with respect to `JOBS` and `WORK_PER_JOB`?

To simplify the command, I wrote the script:

```bash
#!/bin/bash
JOB=${1?Error: no JOBS given}
WORK=${2?Error: no WORK_PER_JOB given}
make JOBS=$JOB WORK_PER_JOB=$WORK clean serial
export PATH="~/Documents/ENGI9875/lab2:$PATH"
echo $PATH
for i in $(seq 100)
do
        serial | awk '{ $7 }' | tee -a initial_serial_times.dat
done

for i in {5,10,100}
do 
        head -$i initial_serial_times.dat | ministat -A -s
done
```

And save it as `lab.sh`

At first,  we increased `WORK_PER_JOB` solely, set `JOBS=10 WORK_PER_JOB=100`:

```shell
$ ./lab.sh 10 100
```

The result became: 

```shell
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x   5         4.132         4.826         4.301          4.38    0.26833841
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x  10         3.982         4.826         4.337        4.3553    0.24705737
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x 100          3.52        28.522         4.426       5.43476     3.1636723
```

Then set `JOBS=100 WORK_PER_JOB=10`

```shell
$ ./lab.sh 100 10
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x   5         4.074         6.178         4.524        4.7616    0.83976384
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x  10         4.074        37.292         4.524        7.8591     10.359304
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x 100         3.652        37.318         4.553       5.75477     4.8713627
```

Finally set `JOBS=100 WORK_PER_JOB=100`

```shell
$ ./lab.sh 100 100
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x   5        2.0385        2.5414        2.4017       2.31382    0.25125952
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x  10        2.0385        2.5414        2.4895       2.38999    0.19411224
x <stdin>
    N           Min           Max        Median           Avg        Stddev
x 100        2.0053       15.5879        2.4605      2.672138     1.4997315
```

It is very clear that with the increase of `JOBS` and `WORK_PER_JOB`, the execution time is decreasing. Here are possible reasons:

- there are some basic time consumption. The increased execution number will dilute this effect.
- system schedules better when the execution number increase. 



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