#include <unistd.h> // pid_t, fork()
#include <stdio.h>  // fprintf(), stderr
#include <errno.h>  // errno
#include <string.h> // strerror()
#include <stdlib.h> // exit

/** error-reporting function
 *      to simplify the code in UNIX-style
 * @param msg: message to display in the shell
 */
void unix_error(char *msg)
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

/** error-handling wrapper of fork()
 *      to simplify the code
 * @return pid
 */
pid_t Fork(void)
{
    pid_t pid;
    pid = fork();
    if(pid < 0)
        unix_error("Fork error");
    return pid;
}

