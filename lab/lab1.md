### System call tracing

1. Trace the system calls that are executed by the command `echo "hello"` two times. Explain the differences between these invocations. **Note:** you may find the `wdiff(1)` and `colordiff(1)` programs or the graphical `meld(1)` program helpful in your investigation.

   ```bash
   xy@xy-vm:~$ strace -o echo1.txt echo "Hello"
   xy@xy-vm:~$ strace -o echo2.txt echo "Hello"
   xy@xy-vm:~$ meld echo1.txt echo2.txt 
   ```

   The only difference is the memory address. 	

   ![image-20210131110759283](.\figures\image-20210131110759283.png)

2. Trace the system calls executed by the command `cat /etc/fstab`. Explain the differences between this trace and those of the `echo "hello"` invocations above.

   The memory address is different. Most of systems call are the same, except at the ending, `cat` has system calls such as `openat()`, `read()` to open file and read the content of file, while `echo` only write the content that user typed. 

![image-20210131213828774](.\figures\image-20210131213828774.png)

### System call assembly code

1. Compile the following x86 assembly code into an object file: stuff.s

   ```assembly
   	# The .data section of an object file holds, well, data! Names defined
   	# in this section will appear in the symbol table as global variables.
   	.data
   
   message:
   	.ascii "Hello, world!\n"
   
   	# The .text section contains instructions. The name is "text" for...
   	# historical reasons.
   	.text
   
   	# Here's how we define a function in assembly! This function doesn't
   	# take any parameters, so we just start executing instructions.
   	.global	do_stuff
   
   do_stuff:
   	mov	$message, %eax
   	int	$0x80
   
   	ret
   ```

   ```bash
   $ gcc -c stuff.s
   ```

    Use `objdump(1)` to disassemble the object file. Explain any differences you observe between the disassembly and the original `.S` file.

<img src=".\figures\image-20210201111932443.png" alt="image-20210201111932443" style="zoom:67%;" />

In the disassembled object file, there is no `.ascii` or `.global` section. 

In the `do_stuff` section, the `mov` 's name isn't `$message`, instead it's a address.  In the `message` section, things are totally different from the original object file. The `objdump` translate ASCII code into something syntax correct but nonsense. 



2. Download [main.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/1/main.c) and [Makefile](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/1/Makefile) and use them to compile a complete program. Use `objdump(1)` to disassemble the complete program (hint: what does the command `grep -A` do?). Identify any differences between the `do_stuff` function in this output from that of the `stuff.o` disassembly. Use `nm(1)` to explain these differences.

First of all, put stuff.c, main.c, Makefile into one folder and `make`. 

`grep -A NUM` prints  NUM  lines  of  trailing  context after matching lines.  Places a line containing a group separator (--) between contiguous groups of matches.  As the following result shows, there are only three lines in the `stuff.o` disassembly, while there are much more in the `syscalls` disassembly. Apparently, `do_stuff` appears twice in the `syscalls` disassembly, one is the definition, another is the calling. Using `nm`, we can tell that syscalls symbols include stuff.o symbols, but have many more symbols.

```bash
$ make
clang -Weverything -g   -c -o main.o main.c
main.c:25:14: warning: unused parameter 'argc' [-Wunused-parameter]
int main(int argc, char *argv[])
             ^
main.c:25:26: warning: unused parameter 'argv' [-Wunused-parameter]
int main(int argc, char *argv[])
                         ^
2 warnings generated.
as   -o stuff.o stuff.s
clang main.o stuff.o -o syscalls

$ objdump -D stuff.o > stuff.txt
$ objdump -D syscalls > syscalls.txt

$ grep -A 6 -E 'do_stuff' *.txt
stuff.txt:0000000000000000 <do_stuff>:
stuff.txt-   0:	b8 00 00 00 00       	mov    $0x0,%eax
stuff.txt-   5:	cd 80                	int    $0x80
stuff.txt-   7:	c3                   	retq   
stuff.txt-
stuff.txt-Disassembly of section .data:
stuff.txt-
--
syscalls.txt:  4005da:	e8 4b 00 00 00       	callq  40062a <do_stuff>
syscalls.txt-  4005df:	bf 02 00 00 00       	mov    $0x2,%edi
syscalls.txt-  4005e4:	48 8d 75 d0          	lea    -0x30(%rbp),%rsi
syscalls.txt-  4005e8:	e8 73 fe ff ff       	callq  400460 <clock_gettime@plt>
syscalls.txt-  4005ed:	83 f8 00             	cmp    $0x0,%eax
syscalls.txt-  4005f0:	0f 84 16 00 00 00    	je     40060c <main+0x8c>
syscalls.txt-  4005f6:	bf ff ff ff ff       	mov    $0xffffffff,%edi
--
syscalls.txt:000000000040062a <do_stuff>:
syscalls.txt-  40062a:	b8 40 10 60 00       	mov    $0x601040,%eax
syscalls.txt-  40062f:	cd 80                	int    $0x80
syscalls.txt-  400631:	c3                   	retq   
syscalls.txt-  400632:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
syscalls.txt-  400639:	00 00 00 
syscalls.txt-  40063c:	0f 1f 40 00          	nopl   0x0(%rax)

$ nm stuff.o
0000000000000000 T do_stuff
0000000000000000 d message

$ nm syscalls
000000000060104e B __bss_start
                 U clock_gettime@@GLIBC_2.17
000000000060104e b completed.7698
0000000000601030 D __data_start
0000000000601030 W data_start
00000000004004d0 t deregister_tm_clones
00000000004004c0 T _dl_relocate_static_pie
0000000000400540 t __do_global_dtors_aux
0000000000600e18 t __do_global_dtors_aux_fini_array_entry
000000000040062a T do_stuff
0000000000601038 D __dso_handle
0000000000600e20 d _DYNAMIC
000000000060104e D _edata
0000000000601050 B _end
                 U err@@GLIBC_2.2.5
00000000004006b4 T _fini
0000000000400570 t frame_dummy
0000000000600e10 t __frame_dummy_init_array_entry
000000000040085c r __FRAME_END__
0000000000601000 d _GLOBAL_OFFSET_TABLE_
                 w __gmon_start__
0000000000400720 r __GNU_EH_FRAME_HDR
0000000000400438 T _init
0000000000600e18 t __init_array_end
0000000000600e10 t __init_array_start
00000000004006c0 R _IO_stdin_used
00000000004006b0 T __libc_csu_fini
0000000000400640 T __libc_csu_init
                 U __libc_start_main@@GLIBC_2.2.5
0000000000400580 T main
0000000000601040 d message
                 U printf@@GLIBC_2.2.5
0000000000400500 t register_tm_clones
0000000000400490 T _start
0000000000601050 D __TMC_END__
```



3. Trace the system calls executed by the complete program. Explain any differences from the echo "hello" traces.

```bash
$ strace -o hello.txt echo "hello"
$ strace -o syscalls.txt ./syscalls 
$ meld hello.txt syscalls.txt 
```

![image-20210207230720585](.\figures\image-20210207230720585.png)

The first half parts of these two files respectively are near the same, except for the address. 

However, the second half parts are different to large degree. `syscalls` has a new system call clock_gettime and calls itself, while `hello` has openat, mmap exclusively. And the sequence of system calls is different. 



4. With the complete program’s disassembly open in another window, run the program under lldb(1), breaking execution at the do_stuff function. Single-step to the int $0x80 instruction and print the contents of all registers using the register read command. Interpret these registers, providing the value of the system call number that is being executed and its arguments. Show how the system call number matches what you observed in the system call trace (the arguments are unlikely to match: you will find that they change every time you run the program depending on addresses left in registers at the time of the do_stuff call).