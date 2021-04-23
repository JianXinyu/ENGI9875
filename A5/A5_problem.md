# WRITE A FILESYSTEM

23:59 on 31 Mar 2021

Implement a simple filesystem in userspace using [FUSE](https://github.com/libfuse/libfuse).

## Objective

Using the material we’ve covered in [Lecture 16 ("File permissions")](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/16/) and [Lecture 17 ("Filesystems")](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/17/), you will write a simple userspace filesystem using [FUSE](https://github.com/libfuse/libfuse). This will help you:

- understand blocks, inodes and superblocks,
- practice using `mmap(2)` and pointer arithmetic, and
- practice using a practical object-oriented C API.

## Details

This assignment will, again, require you to implement some limited functionality but give you the option of additional features to implement.

### FUSE versions

The transition from FUSE v2 to v3 isn’t complete, particuarly in the version of Ubuntu that we use for out Gradescope autograder and on LabNet computer. So, in this assignment, we’ll use FUSE v2. For this reason, you may find it more helpful to consult the (quite extensive) documentation comments in [fuse_lowlevel.h](https://github.com/libfuse/libfuse/blob/fuse_2_9_bugfix/include/fuse_lowlevel.h#L150) rather than the [online documentation](http://libfuse.github.io/doxygen) (which focuses on FUSE v3). To install FUSE v2 on a few operating systems:

|      OS      |        INSTALLATION COMMAND         |                            NOTES                             |
| :----------: | :---------------------------------: | :----------------------------------------------------------: |
|  Arch Linux  |       `sudo pacman -S fuse2`        |             From Arch website — not tested by me             |
|   FreeBSD    |   `sudo pkg install fusefs-libs`    |         Not technically supported but seems to work          |
|    MacOS     |     `brew cask install osxfuse`     | Uses FUSE compatibility layer (`FUSE_USE_VERSION` in assign4.h) |
| Ubuntu Linux | `sudo apt install fuse libfuse-dev` |                      Should "Just Work"                      |

### Getting started

First of all, you should sketch out the design of your filesystem data structures. This won’t be submitted, so you may use whatever format (UML, pseudocode/Python) you like for this work. Once you’re ready to start putting your thoughts into code, please see the following boilerplace code that I have provided you with some to get you going:

- [fuse.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/fuse.c) contains a `main()` function that sets up `libfuse` appropriately; you shouldn’t need to modify this
- [assign4.h](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/assign4.h) declares the only function that you **absolutely must** implement in order for your assignment to compile: a function that provides a pointer to a `fuse_lowlevel_ops` structure (which itself contains pointers to your FS implementation functions)
- [assign4.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/assign4.c) contains function stubs for many common filesystem operations, spelled in the manner that FUSE expects — you will need to flesh out the implementation of (at least some of) these functions
- [example.c](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/example.c) contains an example of a trivial (but working) read-only FUSE filesystem — this level of functionality is enough to get you part of the [core functionality](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/#_core_functionality) marks!
- [Makefile](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/Makefile) should help you compile it all

Once you’ve compiled the assignment, you can run it as follows:

```console
$ mkdir tmp                       # empty directory to mount your filesystem in
$ ./fuse -d tmp myfilesystem.dat  # run FUSE with debug output information
FUSE library version: 2.9.4
unique: 1, opcode: INIT (26), nodeid: 0, insize: 56, pid: 0
INIT: 7.23
flags=0x0003fffb
max_readahead=0x00020000
assign4_init 'filesystem.dat'
   INIT: 7.19
   flags=0x00000001
   max_readahead=0x00020000
   max_write=0x00020000
   max_background=0
   congestion_threshold=0
   unique: 1, success, outsize: 40
unique: 2, opcode: ACCESS (34), nodeid: 1, insize: 48, pid: 1766
assign4_access ino=1 mask=4
   unique: 2, error: -38 (Function not implemented), outsize: 16
[...]
```

In a separate terminal, you can then interact with your mounted filesystem using normal filesystem commands like `ls(1)`:

```console
$ ls tmp
ls: cannot access 'tmp': Function not implemented
```

(this error shows up because the `assign4_getattr()` function sends an `ENOSYS` error, which means "Function not implemented")

When you’re done with your filesystem, run `fusermount -u tmp`.

## Requirements

### Filesystem operations

In general, FUSE operations will cause one of your FUSE functions to be called. Rather than returning a specific result value, however, you will need to invoke a callback, either `fuse_reply_err()` or a reply meaningful to that operation (e.g., see how `example_getattr()` calls `fuse_reply_err()` on error and `fuse_reply_attr()` on success). **It is essential that each of your functions call one and only one FUSE reply function.**

### Core functionality

At minimum, your filesystem should expose a directory called `assignment` that contains the following read-only files:

- `username`

  your Memorial username

- `features`

  a text file containing a description of the optional features you have implemented

That is, while your `fuse` binary is running (see ["Getting started"](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/assignment/5/#_getting_started)), you should be able to execute commands such as:

```console
$ cat tmp/assignment/username
p15jra
$ cat tmp/assignment/features
I have implemented the following optional features:

 - directory listing
 - file creation and unlinking
```

This directory may be served from objects in memory rather than being backed by a real file. Implementing this core functionality will require implementing filesystem operations such as `lookup` and `getattr`.

### Optional functionality

In addition to the core functionality described above, you should implement at least four of the following optional features to receive full marks. **Some of these features have interdependencies.** Implementing some, but not all, of the following features will result in partial credit being awarded. Implementing extra features correctly may help your mark if any of your other features are incorrect (we will take the best four features), but you cannot score more than 100%.

- Directory listing

  Implement the `readdir` operation to support the use of tools like `ls(1)`:

```console
$ ls tmp
assignments
$ ls tmp/assignments
features   username
```

- Directory creation and removal

  We should be able to create empty directories (anywhere but in the `assignment` directory) using `mkdir(1)`/`mkdir(2)` and remove them with `rmdir(1)`/`rmdir(2)`. Direction creation and removal will be confirmed with `stat(2)` and `access(2)`:

```console
$ mkdir tmp/some_directory_name
$ stat tmp/some_directory_name
16777221 8613258905 drwxr-xr-x 2 jon staff 0 64 "Mar  6 15:32:22 2018" "Mar  6 15:32:22 2018" "Mar  6 15:32:22 2018" "Mar  6 15:32:22 2018" 4194304 0 0 directory_name
$ rmdir tmp/some_directory_name
$ stat tmp/some_directory_name
stat: some_directory_name: stat: No such file or directory
```

- File creation and unlinking

  We should be able to create empty files (anywhere but in the `assignment` directory) using `touch(1)` and/or `open(2)` with `O_CREAT`. We should also be able to unlink files with `rm(1)` and `unlink(2)`. File creation and unlinking will be confirmed with `stat(2)` and `access(2)`.

- File modification

  We should be able to modify files (anywhere but in the `assignment` directory) using `write(2)`. The results of these modifications should be visible via file inspection tools such as `cat(1)` and `grep(1)`.

- Permission manipulation

  Implement uid/gid manipulation and permission mode setting. This will be tested using `chmod(1)`/`chmod(2)` and `stat(2)`/`fstat(2)`. You may need to use the FUSE `allow_other` option when testing this feature.

- Permission checking

  Implement permission checking to prevent users from performing unauthorized operations on files and directories. This feature can only be tested (and credit assigned) if you have implemented permission manipulation. You may need to use the FUSE `allow_other` option when testing this feature.

- Persistence (worth two)

  Store the contents of your filesystem in the backing file specified at the command line. The first invocation of the `fuse` assignment binary should initialize the backing file (in the `init` operation); modifications made to files or directories should be persisted to this backing file. Subsequent invocations of the assignment `fuse` binary should show the results of these modifications having been persisted. To receive full credit for this feature, you must have implemented **at least two** of the modification-related features above.

## Deliverables

To complete this assignment, you must submit source code via Gradescope.