# WRITE A SHELL

23:59 on 1 Mar 2021

Implement a simple shell program with a few features.

## Objective

Using the background and techniques described in [Lecture 6 ("Shells")](https://memorialu.gitlab.io/Engineering/ECE/Teaching/operating-systems/website/lecture/6/) (and a little bit from lecture 7, "Inter-process communication"), you will write a simple shell program that can execute programs based on user input. This will help you:

- understand process relationships and the process tree more clearly,
- practice using the process control API and
- practice using basic Unix IPC primitives.

## Details

This assignment requires you to implement some core functionality and also affords you the choice of additional optional functionality.

### Core functionality

Your shell program **must** display a prompt, accept user input in the form of a command line and execute the program requested by the user. The first token in the command line is the name of the program to be executed: if it is found in the `PATH` environment variable then it should be executed, otherwise a warning should be printed. The rest of the command line should be passed to the program as arguments. Your shell program should wait for the started program to finish and print out why it stopped running (e.g., "exited with code 42" or "terminated by SIGKILL").

You may find the `strsep(3)` function to be helpful, although you are welcome to use whatever parsing techniques you prefer.

### Optional functionality

You must implement **at least four** of the following additional functional requirements. You may implement more than four; the best four will be used when evaluating your assignment submission.

- Cancellation

  Handle Ctrl+C input, using it to terminate a currently-executing child process rather than the shell program itself.

- History

  Implement a `history` built-in command that will display all of the commands that the user has typed in this session.

- Line editing

  Using terminal raw mode (see: [a helpful tutorial](https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html)) handle the left and right arrow keys, together with delete and backspace, to allow interactive editing of the command line. Handle the up and down arrow keys, using them to recall previously-executed command lines.

- Pipes

  Allow the output of one program to be attached to the input of another program using the pipe (`|`) character.

- Redirection

  Allow a program’s standard output (`stdout`) or standard error output (`stderr`) to be redirectored to a file using `>` or `2>`, respectively.

- Suspension

  Handle Ctrl+Z input, using it to suspend a currently-executing child process rather than the shell program itself.

- Tab completion

  When the user types part of a filename and presses Tab, automatically complete the filename if the prefix entered thus far is unique.

- Variables

  Using the syntax employed by the Bourne shell, add support for setting local variables that are only used by the shell itself (`FOO=bar`), setting environment variables that are passed on to executed programs (`export FOO=bar`) and retrieving the value of either kind of variable to pass to executed programs (`echo ${FOO}`).

## Deliverables

To complete this assignment you must submit source code to Gradescope.