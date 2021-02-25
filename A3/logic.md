# Overview of Unix Shells

Reference: CMU CS213 [Lab5](http://csapp.cs.cmu.edu/3e/labs.html)

A shell is an interactive command-line interpreter that runs programs on behalf of the user. A shell repeatedly prints a prompt, waits for a command line on `stdin`, and then carries out some action, as directed by the contents of the command line.

The command line is a sequence of ASCII text words delimited by whitespace. 

- The first word in the command line is either 

  - the name of a built-in command or
    - the shell immediately executes the command in the current process.
  - the pathname of an executable file. 
    - the shell forks a child process, then loads and runs the program in the context of the child.

- The remaining words are command-line arguments. 

  

The child processes created as a result of interpreting a single command line are known collectively as a **job**. In general, a job can consist of multiple child processes connected by Unix pipes.  

- If the command line ends with an ampersand ”&”, then the job runs in the background, which means that the shell does not wait for the job to terminate before printing the prompt and awaiting the next command line. 
- Otherwise, the job runs in the foreground, which means that the shell waits for the job to terminate before awaiting the next command line. 

Thus, at any point in time, at most one job can be running in the foreground. However, an arbitrary number of jobs can run in the background.

# Code Logic

1. display a prompt

2. read the input

3. call `parseline()` 

   - parse the space-separated command-line arguments
- build the `argv` vector
   - return $bg$
  - if $argv[-1] == \&$, return 1, representing execute the program in the background
     - otherwise, return 0, run the program in the foreground (shell waits for it to complete)
   
4. if $argv[0] == NULL$: no input, simply return  

5. call `builtin_cmd()`

  - if $arg[0]$ is the name of a built-in shell command: interpret immediately, then return 1 
  - if $argv[0]$ is an executable file: return 0

6. if return 0:

   - `fork()`: create a child process
   - `execve()`
   - bg
     - if $bg$: return to the next iteration
     - if not: use `waitpid()` to wait for the job to terminate. When the job terminates, the shell goes on to the next iteration

   

   