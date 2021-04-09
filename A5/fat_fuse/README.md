File system using FUSE
=======================

Wojciech Pratkowiecki

Operating Systems - P2 - winter semester 2016/17

-----------------

Repository for the implementation of the driver to support the FAT16 file system. The driver works with the ** Fuse ** (Filesystem in Userspace) interface, which allows you to create your own file systems in user mode.

---------------

Use of the driver
-------

The repository contains a `Makefile`, which allows you to compile your project with the` make` command.

As the purpose of the program is to support the FAT16 file system, you need a FAT16 image. The repository contains the script `make-fat16.sh`, which creates a` fs-image.raw` file of a 100MB FAT16 filesystem with a complex structure.

Then run the `fusermount` program giving it as arguments the path to the FAT16-formatted filesystem and the directory path where the system image will be mounted, for example:

`./filesystem fs-image.raw mountpoint /`

The specified directory contains a file system image. It can be navigated like a typical directory using common commands such as `ls, cd, stat, cat`

To unmount a file system, use one of the ready-made tools. For example:

`fusermount -u mountpoint`

The directory can be cleaned of files created during compilation with the command `make clean`

------------------------
Useful links:

[Fuse on GitHub] (https://github.com/libfuse/libfuse)

[FAT16 documentation] (http://www.maverick-os.dk/FileSystemFormats/FAT16_FileSystem.html)

[FAT16 Tutorial] (http://www.tavi.co.uk/phobos/fat.html)