## Purpose and outcomes

The goal of this lab is to explore the practical implications of page faults on program behaviour. By the end of this lab you should be able to:

1. write C code to query the number of major and minor page faults in a program or its children,
2. explain the relationship among address space allocation, page dirtying, physical memory use and swapping; and
3. explain *thrashing*.

### Prelab

Answer all of the following questions. Provide evidence for your claims.

1. With reference to the `getrusage(2)` manual page **on Linux**, explain the difference between a *major* and a *minor* page fault as described in `struct rusage`.

   Synopsis:

   ```c
   #include <sys/time.h>
   #include <sys/resource.h>
   int getrusage(int who, struct rusage *usage);
   ```

   >getrusage()  returns  resource usage measures for __who__.

   In the manual, there is no major or minor page fault in `struct rusage`, instead, it has these two fields:

   ```c
   long   ru_minflt;        /* page reclaims (soft page faults) */
   long   ru_majflt;        /* page faults (hard page faults) */
   ```

   They are interpreted as follows:

   >`ru_minflt` is the number of page faults serviced  without  any  I/O  activity; here  I/O  activity is avoided by “reclaiming” a page frame from the list of pages awaiting reallocation.
   >
   >`ru_majflt` is the number of page faults serviced that required I/O activity.

   According to the name, a *minor* page fault is represented by `ru_minflt`, while a *major* page fault is represented by `ru_majflt`. In my understanding, a *minor* page fault means the page is in memory but not allocated to the requesting process or not marked as present in the memory management unit. A major fault means the page in no longer in memory. That's why a *major* page fault requires I/O activities, while a `minor` page fault needn't. 

2. (not for 2020-21W) ~~What is the key difference between the FreeBSD and Linux versions of `getrusage(2)`?~~
3. ~~Implement the following function **for Linux** in a file called `rusage.c`:~~

```C
/**
 * Emit current resource usage information into a string buffer.
 *
 * @param   buffer       a string buffer of at least 1 B
 * @param   buffer_len   the length of @b buffer
 */
void get_rusage_string(char *buffer, size_t buffer_len);
```

4. ~~Complete the implementation of the program [balloon.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/balloon.c) by implementing its `void balloon(size_t)` function. Test this program and demonstrate that `balloon()` uses the requested number of bytes of physical memory, not just address space (and explain the difference).~~

## Procedure

### Setup

1. On a stock Linux installation (e.g., Ubuntu 18, as in LabNet), ensure that you have installed the following libraries and tools:

   - a C++ compiler (`apt install g++` on Ubuntu)
   - the Gtk+ UI toolkit v2 (`libgtk2.0-dev` on Ubuntu)
   - the [CMake](https://cmake.org/) configuration tool (`cmake`)
   - the [Ninja](https://ninja-build.org/) build tool (`ninja-build`)

2. Download the following files:

   1. [CMakeLists.txt](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/CMakeLists.txt)
   2. ~~[balloon.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/balloon.c)~~ (not for 2020-21W)
   3. [main.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/main.c)
   4. [rusage.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/rusage.c)
   5. [setup-build.sh](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/setup-build.sh)
   6. command-line UI implementation:
      1. [cmdline/CMakeLists.txt](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/cmdline/CMakeLists.txt)
      2. [cmdline/ui.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/cmdline/ui.c)
   7. GUI implementation: (not for 2020-21W)
      1. ~~[gui/CMakeLists.txt](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/gui/CMakeLists.txt)~~
      2. ~~[gui/gtk-ui.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/gui/gtk-ui.c)~~
   8. header files:
      1. [include/rusage.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/include/rusage.h)
      2. [include/ui.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/3/include/ui.h)

3. Set up the build by running the `setup-build.sh` script.

   ```bash
   $ chmod +x setup-build.sh
   $ ./setup-build.sh
   + mkdir -p build
   + cd build
   + cmake -G Ninja ..
   -- The C compiler identification is GNU 7.5.0
   -- The CXX compiler identification is GNU 7.5.0
   -- Check for working C compiler: /usr/bin/cc
   -- Check for working C compiler: /usr/bin/cc -- works
   -- Detecting C compiler ABI info
   -- Detecting C compiler ABI info - done
   -- Detecting C compile features
   -- Detecting C compile features - done
   -- Check for working CXX compiler: /usr/bin/c++
   -- Check for working CXX compiler: /usr/bin/c++ -- works
   -- Detecting CXX compiler ABI info
   -- Detecting CXX compiler ABI info - done
   -- Detecting CXX compile features
   -- Detecting CXX compile features - done
   -- Looking for pthread.h
   -- Looking for pthread.h - found
   -- Looking for pthread_create
   -- Looking for pthread_create - not found
   -- Looking for pthread_create in pthreads
   -- Looking for pthread_create in pthreads - not found
   -- Looking for pthread_create in pthread
   -- Looking for pthread_create in pthread - found
   -- Found Threads: TRUE  
   -- Configuring done
   -- Generating done
   -- Build files have been written to: /home/xy/ENGI9875/lab3/build
   + ninja
   [6/6] Linking C executable lab
   + echo ok
   ok
   ```

4. Build the project by running `ninja` within the `build` directory.

   ```bash
   $ cd build/
   $ ninja -f build.ninja 
   ninja: no work to do.
   ```

5. Generate a text file containing 10 MiB worth of pseudo-random data:

   ```console
   $ dd if=/dev/urandom bs=1024 count=10240 | base64 > random.txt
   ```

### Command-line `lab` tool

1. Implement the `get_rusage_string()` in `rusage.c`:

   ```C
   /**
    * Emit current resource usage information into a string buffer.
    *
    * @param   buffer       a string buffer of at least 1 B
    * @param   len   the length of @b buffer
    */
   void get_rusage_string(char *buffer, size_t len)
   {
           struct rusage *rptr = malloc(sizeof(struct rusage));
           int rt;
           if((rt = getrusage(RUSAGE_SELF, rptr)) !=0){
                   fprintf(stdout, "getrusage() error: %s\n", strerror(errno));
                   exit(1);
           }
           snprintf(buffer, len,   "user CPU time used             : %ld\nsystem CPU time used             : %ld\nmaximum resident set size        : %ld\nintegral shared memory size      : %ld\n"
                                   "integral unshared data size    : %ld\nintegral unshared stack size     : %ld\nminor faults                     : %ld\nmajor faults                     : %ld\n"
                                   "swaps                          : %ld\nblock input operations           : %ld\nblock output operations          : %ld\nIPC messages sent                : %ld\n"
                                   "IPC messages received          : %ld\nsingals received         : %ld\nvoluntary context switches       : %ld\ninvoluntary context switches     : %ld\n",
                                   rptr->ru_utime.tv_usec, rptr->ru_stime.tv_usec, rptr->ru_maxrss, rptr->ru_ixrss,
                           rptr->ru_idrss, rptr->ru_isrss, rptr->ru_minflt, rptr->ru_majflt,
                                   rptr->ru_nswap, rptr->ru_inblock, rptr->ru_oublock, rptr->ru_msgsnd,
                           rptr->ru_msgrcv, rptr->ru_nsignals, rptr->ru_nvcsw, rptr->ru_nivcsw);
   
   }
   ```

2. Execute the `lab` binary and record all output.

   ```bash
   xy@xy-vm ~/D/build> ./lab
   ----
   beginning of main()
   ----
   rusage:
   user CPU time used		: 885
   system CPU time used		: 0
   maximum resident set size	: 2040
   integral shared memory size	: 0
   integral unshared data size	: 0
   integral unshared stack size	: 0
   minor faults			: 90
   major faults			: 0
   swaps				: 0
   block input operations		: 0
   block output operations		: 0
   IPC messages sent		: 0
   IPC messages received		: 0
   singals received		: 0
   voluntary context switches	: 0
   involuntary context switches	: 0
   
   ----
   after ui_init()
   ----
   rusage:
   user CPU time used		: 960
   system CPU time used		: 0
   maximum resident set size	: 2040
   integral shared memory size	: 0
   integral unshared data size	: 0
   integral unshared stack size	: 0
   minor faults			: 94
   major faults			: 0
   swaps				: 0
   block input operations		: 0
   block output operations		: 0
   IPC messages sent		: 0
   IPC messages received		: 0
   singals received		: 0
   voluntary context switches	: 0
   involuntary context switches	: 0
   
   Press Enter to exit...
   ----------------------
   after starting UI thread
   ----------------------
   rusage information
   user CPU time used		: 1531
   system CPU time used		: 0
   maximum resident set size	: 2040
   integral shared memory size	: 0
   integral unshared data size	: 0
   integral unshared stack size	: 0
   minor faults			: 98
   major faults			: 0
   swaps				: 0
   block input operations		: 0
   block output operations		: 0
   IPC messages sent		: 0
   IPC messages received		: 0
   singals received		: 0
   voluntary context switches	: 2
   involuntary context switches	: 0
   
   ----
   after UI thread complete
   ----
   rusage:
   user CPU time used		: 2185
   system CPU time used		: 0
   maximum resident set size	: 2040
   integral shared memory size	: 0
   integral unshared data size	: 0
   integral unshared stack size	: 0
   minor faults			: 99
   major faults			: 0
   swaps				: 0
   block input operations		: 0
   block output operations		: 0
   IPC messages sent		: 0
   IPC messages received		: 0
   singals received		: 0
   voluntary context switches	: 3
   involuntary context switches	: 0
   ```

3. Plot page faults — major and minor — against total CPU time.

   														![image-20210328205539260](.\image-20210328205539260.png)
  
  ![image-20210328205619111](.\image-20210328205619111.png)
  
  As we can see, there are no major faults, only minor faults. The reason might be that we run the code just after we created the random.txt, the pages are still in the memory. Thus, there is no I/O activity. 

### ~~GUI~~

1. ~~Execute the `lab` binary again with the `LD_PRELOAD` environment variable set to `gui/libgui.so`. Record all outputs and plot page faults as above. Explain differences with previous tool invocations. What effect does opening (and cancelling) the exit dialog have on rusage?~~
2. ~~Implement `get_command_rusage()` using `wait4(2)` and test it.~~
3. ~~Complete the `on_click_run_*()` functions in `gui/gtk-ui.c` to make them invoke the C compiler (`cc(1)`) and `grep(1)` via `get_command_rusage()`. Recording all outputs and plotting page faults as above, explain the differences (where they exist) between invocations of `cc(1)` and `grep(1)` vs between running `grep(1)` against small and large files.~~

### ~~Memory pressure~~

1. ~~Use the `top(1)` command to inspect the background memory usage of the computer that you are currently using. How much memory is available? How much swap space?~~
2. ~~Run `/usr/bin/time -v gdb ./lab` to run the lab binary under the `gdb(1)` debugger. Record outputs and plot page faults as above.~~
3. ~~Using the `balloon` application that you completed in your pre-lab preparation, squeeze other data out of memory until the available space is nearly exhausted — in fact, run until the OS won’t let you balloon any larger (the machines we’ll be working on aren’t configured to use swap, so we won’t be able to observe the effect of swapping data out). Explain your observations along the way.~~