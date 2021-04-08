# Assignment 5 Write a filesystem Usage Doc

## Set-up

1. open a terminal, `cd` to current directory

```bash
$ make
$ mkdir tmp
$ ./fuse -d tmp myfilesystem.dat 
```

2. open another terminal, `cd` to current directory

### Core functionality

```bash
xy@xy-vm:~/ENGI9875/A5$ cat tmp/assignment/username
xjian
xy@xy-vm:~/ENGI9875/A5$ cat tmp/assignment/features
I have implemented the following optional features:
 - Directory listing
 - File modification
 - File creation
 - Directory creation
```

### Optional functionality

**Directory listing**

```bash
xy@xy-vm:~/ENGI9875/A5$ ls tmp
assignment
xy@xy-vm:~/ENGI9875/A5$ ls tmp/assignment/
features  username
```

**Directory creation and removal**

**File creation and unlinking**

**File modification**