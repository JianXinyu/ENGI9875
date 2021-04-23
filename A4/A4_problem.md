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



# Autograder Results

AUTOGRADER SCORE 11.0 / 11.0

1.1) Executable 'assign4' exists (1.0/1.0)

1.2) assign4 can be executed (1.0/1.0)

```
FIFO 4
1
2
3
stdout:
---
algorithm> FIFO 4
page>1
1F
page>2
2F
page>3
3F
page>
All done, goodbye!

---


stderr:
---

---
```

2.1) FIFO w/4 frames: access pages 1, 2 and 3 (1.0/1.0)

```
FIFO 4
1
2
3
stdout:
---
algorithm> FIFO 4
page>1
1F
page>2
2F
page>3
3F
page>
All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> FIFO 4
no match:   page>1
MATCH:      1F 

Looking for "2F"...
no match:   page>2
MATCH:      2F 

Looking for "3F"...
no match:   page>3
MATCH:      3F
```

2.2) FIFO w/2 frames: access pages 1, 2 and 3 (3 should evict 1) (1.0/1.0)

```
FIFO 2
1
2
3
stdout:
---
algorithm> FIFO 2
page>1
1F
page>2
2F
page>3
3F E1
page>
All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> FIFO 2
no match:   page>1
MATCH:      1F 

Looking for "2F"...
no match:   page>2
MATCH:      2F 

Looking for "3F E1"...
no match:   page>3
MATCH:      3F E1
```

2.3) FIFO w/5 frames: 1 7 7 8 0 2 2 5 3 6 7 4 1 6 2 6 4 5 0 9 (1.0/1.0)

```
FIFO 5
1
7
7
8
0
2
2
5
3
6
7
4
1
6
2
6
4
5
0
9
stdout:
---
algorithm> FIFO 5
page>1
1F
page>7
7F
page>7
7NF
page>8
8F
page>0
0F
page>2
2F
page>2
2NF
page>5
5F E1
page>3
3F E7
page>6
6F E8
page>7
7F E0
page>4
4F E2
page>1
1F E5
page>6
6NF
page>2
2F E3
page>6
6NF
page>4
4NF
page>5
5F E6
page>0
0F E7
page>9
9F E4
page>
All done, goodbye!

---


stderr:
---

---
```

3.1) LRU w/4 frames: access pages 1, 2 and 3 (1.0/1.0)

```
LRU 4
1
2
3
stdout:
---
algorithm> LRU 4
page>1
1F
page>2
2F
page>3
3F
page>
All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> LRU 4
no match:   page>1
MATCH:      1F 

Looking for "2F"...
no match:   page>2
MATCH:      2F 

Looking for "3F"...
no match:   page>3
MATCH:      3F
```

3.2) LRU w/2 frames: access pages 1 2 1 3 (3 should evict 2) (1.0/1.0)

```
LRU 2
1
2
1
3
stdout:
---
algorithm> LRU 2
page>1
1F
page>2
2F
page>1
1NF
page>3
3F E2
page>
All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> LRU 2
no match:   page>1
MATCH:      1F 

Looking for "2F"...
no match:   page>2
MATCH:      2F 

Looking for "1NF"...
no match:   page>1
MATCH:      1NF 

Looking for "3F E2"...
no match:   page>3
MATCH:      3F E2
```

3.3) LRU w/5 frames: 1 7 7 8 0 2 2 5 3 6 7 4 1 6 2 6 4 5 0 9 (1.0/1.0)

```
LRU 5
1
7
7
8
0
2
2
5
3
6
7
4
1
6
2
6
4
5
0
9
stdout:
---
algorithm> LRU 5
page>1
1F
page>7
7F
page>7
7NF
page>8
8F
page>0
0F
page>2
2F
page>2
2NF
page>5
5F E1
page>3
3F E7
page>6
6F E8
page>7
7F E0
page>4
4F E2
page>1
1F E5
page>6
6NF
page>2
2F E3
page>6
6NF
page>4
4NF
page>5
5F E7
page>0
0F E1
page>9
9F E2
page>
All done, goodbye!

---


stderr:
---

---
```

4.1) Clock w/3 frames: access 1 2 3 2 4 5 (1.0/1.0)

```
CLOCK 3
1
2
3
2
4
5
stdout:
---
algorithm> CLOCK 3
page>1
1F
page>2
2F
page>3
3F
page>2
2NF
page>4
4F E1
page>5
5F E2
page>
All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> CLOCK 3
no match:   page>1
MATCH:      1F 

Looking for "2F"...
no match:   page>2
MATCH:      2F 

Looking for "3F"...
no match:   page>3
MATCH:      3F 

Looking for "2NF"...
no match:   page>2
MATCH:      2NF 

Looking for "4F E1"...
no match:   page>4
MATCH:      4F E1 

Looking for "5F E2"...
no match:   page>5
MATCH:      5F E2
```

4.2) Clock w/4 frames: access 1 2 3 4 2 3 2 1 5 1 5 6 (1.0/1.0)

```
CLOCK 4
1
2
3
4
2
3
2
1
5
1
5
6
stdout:
---
algorithm> CLOCK 4
page>1
1F
page>2
2F
page>3
3F
page>4
4F
page>2
2NF
page>3
3NF
page>2
2NF
page>1
1NF
page>5
5F E1
page>1
1F E2
page>5
5NF
page>6
6F E3
page>
All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> CLOCK 4
no match:   page>1
MATCH:      1F 

Looking for "2F"...
no match:   page>2
MATCH:      2F 

Looking for "3F"...
no match:   page>3
MATCH:      3F 

Looking for "4F"...
no match:   page>4
MATCH:      4F 

Looking for "2NF"...
no match:   page>2
MATCH:      2NF 

Looking for "3NF"...
no match:   page>3
MATCH:      3NF 

Looking for "2NF"...
no match:   page>2
MATCH:      2NF 

Looking for "1NF"...
no match:   page>1
MATCH:      1NF 

Looking for "5F E1"...
no match:   page>5
MATCH:      5F E1 

Looking for "1F E2"...
no match:   page>1
MATCH:      1F E2 

Looking for "5NF"...
no match:   page>5
MATCH:      5NF 

Looking for "6F E3"...
no match:   page>6
MATCH:      6F E3
```

5.1) Belady w/3 frames: access pages 1 2 3 4 1 2 (4 should evict 3) (1.0/1.0)

```
BELADY 3
1
2
3
4
1
2
stdout:
---
algorithm> BELADY 3
['1', '2', '3', '4', '1', '2']
page>1F
page>2F
page>3F
page>4F E3
page>1NF
page>2NF
page> All done, goodbye!

---


stderr:
---

---


Looking for "1F"...
no match:   algorithm> BELADY 3
no match:   ['1', '2', '3', '4', '1', '2']
MATCH:      page>1F 

Looking for "2F"...
MATCH:      page>2F 

Looking for "3F"...
MATCH:      page>3F 

Looking for "4F E3"...
MATCH:      page>4F E3 

Looking for "1NF"...
MATCH:      page>1NF 

Looking for "2NF"...
MATCH:      page>2NF
```