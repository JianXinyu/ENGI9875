1. One source of documentation for Unix commands, system calls and library functions is the manual, made available as online *manual pages*.

   1. Open a command-line terminal and execute the command `man 1 man`. What does this command’s output tell you is the purpose of the `man` command?

      *man* is an interface to the system reference manuals. Specifically, *man* is  the  system's  manual pager. It's basically a command to access the system document.

      

   2. Referencing either the [FreeBSD handbook](https://www.freebsd.org/doc/en_US.ISO8859-1/books/handbook/basics-more-information.html) or the [Linux reference at die.net](https://linux.die.net/man), explain the difference between sections 2 and 3 of the Unix manual.

      Section 2 of the manual describes the Linux system calls. A system call is an entry point into the Linux kernel.

      Section 3 of the manual describes all library functions excluding the library functions (system call wrappers) described in section 2, which implement system calls.

      Many of the functions described in the section are part of the Standard C Library (*libc*).

      

   3. What is the difference between `write(1)` and `write(2)`?

      `write(1)` is a user command, a utility allows you to communicate with other users, by copying lines from  your terminal to theirs.

      `write(2)` is a system call, whose definition is 

      `ssize_t  write(int  fd,  const void *buf, size_t count);`

      It writes up to *count* bytes from the buffer starting  at  *buf* to the file referred to by the file descriptor *fd*.

      

2. What does the Linux `strace(1)` command do?

   to trace system calls and signals

   

3. On a 64b Linux system, use `locate(1)` to find the file `unistd_64.h`.

   1. How many instances of this file do you find?

      4 instances

      ![image-20210130150913755](A:\MUN\Course\2021Winter\ENGI9875-Embedded-and-Real-Time-Operating-System-Design\ENGI9875\lab\figures\image-20210130150913755.png)

   2. Which file is most appropriate for your use in this lab? Why? (you may find the `uname(1)` program to be helpful – how?)

      The first one - /usr/include/x86_64-linux-gnu/asm/unistd_64.h. Since this is a header file that may be used internally by `sys/syscall.h`

      `uname` can print system information. So I can use it to get the architecture type.

      ![image-20210130154103910](A:\MUN\Course\2021Winter\ENGI9875-Embedded-and-Real-Time-Operating-System-Design\ENGI9875\lab\figures\image-20210130154103910.png)

      

   3. What are the system call numbers for `open(2)`, `read(2)`, `write(2)` and `close(2)` on the x86-64 architecture?

      `open(2)`  0

      `read(2) ` 1

      `write(2)` 2

      `close(2)` 3

      

4. Consulting Linux’s inline documentation for the [`syscall`](https://github.com/torvalds/linux/blob/master/arch/x86/entry/entry_64.S#L107), [`sysenter`](https://github.com/torvalds/linux/blob/master/arch/x86/entry/entry_32.S#L882) and [legacy system call entry](https://github.com/torvalds/linux/blob/master/arch/x86/entry/entry_32.S#L1044) instructions, record which registers are used to hold the system call number and arguments for each system call mechanism:

|                    | `SYSCALL` | `SYSENTER` | LEGACY |
| :----------------: | :-------: | :--------: | :----: |
| System call number |    rax    |    eax     |  eax   |
|     Argument 1     |    rdi    |    ebx     |  ebx   |
|     Argument 2     |    rsi    |    ecx     |  ecx   |
|     Argument 3     |    rdx    |    edx     |  edx   |
|     Argument 4     |    r10    |    esi     |  esi   |
|     Argument 5     |    r8     |    edi     |  edi   |
|     Argument 6     |    r9     |  0(%ebp)   |  ebp   |