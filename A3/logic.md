1. display a prompt

2. read the input

3. parse the command line. call a `parseline()` function

   - parse the space-separated command-line arguments

   - build the `argv` vector

     - $argv[0]$ has three possibilities:
       - `NULL`: no input, simply return  
       - the name of a built-in shell command: just interpret
       - an executable file: load the file, run in the context of a child process

     - $argv[-1]$ has two possibilities:
       - $\&$:  execute the program in the background
       - otherwise: run the program in the foreground (shell waits for it to complete)

4. process `argv`
   - if $argv[0]$ is a build in command, interpret immediately
   - if $argv[0]$ is an executable file, use `waitpid()` to wait for the job to terminate. When the job terminates, the shell goes on to the next iteration

