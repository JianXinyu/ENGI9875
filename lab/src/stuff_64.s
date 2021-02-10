        .data
message:
        .ascii "Xinyu Jian\n"

        .text

        .global do_stuff

do_stuff:
        mov     $1, %rax        # write system call
        mov     $1, %rdi        # stdout file descriptor
        mov     $message, %rsi  # bytes to write
        mov     $11, %rdx       # number of bytes to write
        syscall			# perform system call
	ret
