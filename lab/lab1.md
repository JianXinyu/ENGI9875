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

In the `do_stuff` section, the `mov` 's name isn't `$message`, instead it's a address.  In the `message` section, 

things are totally different from the original object file. 



2. Download [main.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/1/main.c) and [Makefile](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lab/1/Makefile) and use them to compile a complete program. Use `objdump(1)` to disassemble the complete program (hint: what does the command `grep -A` do?). Identify any differences between the `do_stuff` function in this output from that of the `stuff.o` disassembly. Use `nm(1)` to explain these differences.

