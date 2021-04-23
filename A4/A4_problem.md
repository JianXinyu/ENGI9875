# PAGE REPLACEMENT

23:59 on 26 Mar 2021

Implement some page replacement algorithms.

## Objective

Write a program that, when given a page replacement algorithm, number of frames and a stream of page accesses, will output information about page replacement. Your program should accept input on standard input that first provides the algorithm and number of frames (on the first line), then one page number per line, e.g.:

```console
FIFO 2
1
2
3
4
```

## Algorithms

Your code should be implement the following page replacement algorithms, identified by these string names:

|                             NAME                             |                         DESCRIPTION                          |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| [BELADY](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/15/#30) | Belady’s optimal algorithm. May require reading ahead (i.e., inputting many page accesses before making decisions about the current page). |
| [CLOCK](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/15/#38) |              Clock algorithm (access bit only).              |
| [FIFO](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/15/#36) |                     First-in first-out.                      |
| [LRU](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/15/#33) |                     Least recently used.                     |

## Details

For each page number read by your program, it should output the same page number, followed by a `F` if the access causes a page fault or `NF` if it does not cause a fault. If there is a page fault, and if that fault evicts an existing resident page, your program should print a space, an `E` and the number of the evicted page on the same line. If the user enters an empty string (or we reach End of File for a text file piped to stdin), your program should terminate. For example, if I pipe the file above into `assign4`, I might see:

```console
Here's some extraneous text at the beginning of the program output.
You can write whatever you like here!

algorithm> FIFO 2
page> 1
1F
page> 2
2F
page> 3
3F E1
page>
All done, goodbye!
```

You may use whatever programming language you like for this assignment: the only compilation requirement is that you include a `Makefile` that describes how to produce an executable file called `assign4`. My autograder will invoke `make` with no arguments and then run the executable file `assign4`. Within that framework, you are free to use whatever language you like (as long as it’s something I can make available in my autograder’s Docker image via the Ubuntu package repository). For example, a `Makefile` for a C-based implementation might look like:

```make
CFLAGS=	-Wall -Werror

assign4: assign4.c
	$(CC) $(CFLAGS) assign4.c -o assign4
```

Whereas a `Makefile` for a Python implementation might be simply:

```make
all:
	chmod +x assign4        # make sure the Python file is executable
```

Submit your work (source files and `Makefile`) to [Gradescope](https://www.gradescope.ca/courses/3478/assignments/13448).