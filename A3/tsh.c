/*
 * A tiny shell program with job control. It supports:
 * 1. Cancellation: ctrl+c
 * 2. Suspension: ctrl+z
 * 3. History
 * 4. Redirection
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXPIDS      6    /* max processes in a job */
#define MAXJID    1<<16   /* max job ID */
#define MAXHIST     100   /* max history commands */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */
/*
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "shell> ";  /* command line prompt */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */
volatile sig_atomic_t pid_flag; /* to track whether SIGCHLD is received */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */

char **history = NULL;  /* store the history commands */
int history_len = 0;    /* record number of commands have been input */
/* End global variables */

/* mainstream functions */
void eval(char *cmdline);
int parseline(const char *cmdline, char **argv);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

/* signal hanlers */
void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);
void sigquit_handler(int sig);

/* history operation */
int history_add(const char *cmdline);
void history_list(void);
void history_free(void);

/* helper functions */
void usage(void);
void unix_error(char *msg);
void app_error(char *msg);

/********************
 * job_t operations
 *******************/
void clearjob(struct job_t *job);   /* Clear the entries in a job struct */
void initjobs(struct job_t *jobs);  /* Initialize the job list */
int maxjid(struct job_t *jobs);		/* Returns largest allocated job ID */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);/* Add a job to the job list */
int deletejob(struct job_t *jobs, pid_t pid);   /* Delete a job whose PID=pid from the job list */
pid_t fgpid(struct job_t *jobs);                /* Return PID of current foreground job, 0 if no such job */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid); /* Find a job (by PID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid);   /* Find a job (by JID) on the job list */
int pid2jid(pid_t pid);             /* Map process ID to job ID */
void listjobs(struct job_t *jobs);  /* Print the job list */

/*********************************************
 * Wrappers for Unix process control functions
 ********************************************/
pid_t Fork(void);
void Kill(pid_t pid, int signum);
pid_t Waitpid(pid_t pid, int *iptr, int options);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);
void Sigfillset(sigset_t *set);
void Sigemptyset(sigset_t *set);
void Sigaddset(sigset_t *set, int signum);
void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

/***********
 * Safe I/O
 ***********/
ssize_t sio_puts(char s[]);
void sio_error(char s[]);
ssize_t Sio_puts(char s[]);
void Sio_error(char s[]);

/*
 * main - The shell's main routine
 */
int main(int argc, char **argv)
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
            case 'h':             /* print help message */
                usage();
                break;
            case 'v':             /* emit additional diagnostic info */
                verbose = 1;
                break;
            case 'p':             /* don't print a prompt */
                emit_prompt = 0;  /* handy for automatic testing */
                break;
            default:
                usage();
        }
    }

    /* Install the signal handlers */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
    Signal(SIGQUIT, sigquit_handler);  /* kill the shell */

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

        /* Read command line */
        if (emit_prompt) {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin)) { /* End of file (ctrl-d) */
            fflush(stdout);
            exit(0);
        }

        /* add this command to the history */
        history_add(cmdline);

        /* Evaluate the command line */
        eval(cmdline);
        fflush(stdout);
        fflush(stdout);
    }

    exit(0); /* control never reaches here */
}

/*
 * eval - Evaluate the command line that the user has just typed in
 *
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.
*/
void eval(char *cmdline)
{
    char *argv[MAXARGS];    /* Argument list execve() */
    char buf[MAXLINE];      /* Holds modified command line*/
    int bg;                 /* Should the job run in bg or fg? */
    pid_t pid;
    sigset_t mask_all, mask_one, prev_one; /* Block bit vector */

    int redir = 0;
    char output[64];

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if(argv[0] == NULL)     /* Ignore empty lines */
        return;

    // find '>'
    for(int i = 0; argv[i] != NULL; i++) {
        if (!strcmp(argv[i], ">")) {
            argv[i] = NULL;
            strcpy(output, argv[i + 1]);
            redir = 1;
        }
    }

    Sigfillset(&mask_all);
    Sigemptyset(&mask_one);
    Sigaddset(&mask_one, SIGCHLD);

    if(!builtin_cmd(argv)){     // executable file
        /* Eliminate "Race" */
        Sigprocmask(SIG_BLOCK, &mask_one, &prev_one); /* Block SIGCHLD */

        if((pid = Fork()) == 0){
            /* Child process */
            setpgid(0, 0); //TODO why?

            //if redirection is needed
//            if (redir) {
//                int fd1 = creat(output, 0644);
//                if( fd1 < 0) {
//                    unix_error("creat fail");
//                }
//                dup2(fd1, STDOUT_FILENO);
//                close(fd1);
//                redir = 0;
//            }

            Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */
            if(execve(argv[0], argv, environ) < 0){
                printf("%s: Command not found. \n", argv[0]);
                return;
            }
        }

        Sigprocmask(SIG_BLOCK, &mask_all, NULL); //TODO when to unblock?
        addjob(jobs, pid, (bg ? BG : FG), cmdline);
        Sigprocmask(SIG_SETMASK, &prev_one, NULL); /* Unblock SIGCHLD */

        if(!bg){            /* the job runs in fg */
            /* wait for SIGCHLD to be received */
            pid_flag = 0;
            while(!pid_flag)
                sigsuspend(&prev_one);
//            waitfg(pid);
//            printf("SHOULDN'T\n");
//            printf("%d\n", jobs[pid2jid(pid)].state);
        }
        else{               /* the job runs in bg */
            printf("[%d] (%d): %s", pid2jid(pid), pid, cmdline);
        }
    }

    return;
}

/*
 * parseline - Parse the command line and build the argv array.
 *
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.
 */
int parseline(const char *cmdline, char **argv)
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
        buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
        buf++;
        delim = strchr(buf, '\'');
    }
    else {
        delim = strchr(buf, ' ');
    }

    while (delim) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* ignore spaces */
            buf++;

        if (*buf == '\'') {
            buf++;
            delim = strchr(buf, '\'');
        }
        else {
            delim = strchr(buf, ' ');
        }
    }
    argv[argc] = NULL;

    if (argc == 0)  /* ignore blank line */
        return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
        argv[--argc] = NULL;
    }
    return bg;
}


/*
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.
 */
int builtin_cmd(char **argv)
{
    if(!strcmp(argv[0], "quit")) // quit command
        exit(0);
    if(!strcmp(argv[0], "&")) // ignore singleton &
        return 1;
    if(!strcmp(argv[0], "echo")){
        if(!strcmp(argv[1], "$$")){
            printf("%d\n", (int)getpid());
        }
        else{
            printf("%s\n", argv[1]);
        }
        return 1;
    }
    if(!strcmp(argv[0], "cd")){
        if(argv[1] == NULL){
            printf("%s\n", getenv("HOME"));
            if(chdir(getenv("HOME")) < 0)
                unix_error("cd error");
        }
        else{
            if(chdir(argv[1]) < 0)
                unix_error("cd error");
        }
        return 1;
    }
    if(!strcmp(argv[0], "jobs")){
        listjobs(jobs);
        return 1;
    }
    if(!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")){
        do_bgfg(argv);
        return 1;
    }
    if(!strcmp(argv[0], "history")){
        history_list();
        return 1;
    }
    return 0;     /* not a builtin command */
}

/*
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)
{
    char *id = argv[1];
    struct job_t *job;

    if(id == NULL){
        printf("%s command requires PID or %%jobID argument\n",argv[0]);
        return;
    }

    if(id[0] == '%'){ //job id
        int jid = atoi(&id[1]);
        job = getjobjid(jobs, jid);
    }
    else if(strspn(id, "0123456789") == strlen(id)){ //pid
        int pid = atoi(id);
        job = getjobpid(jobs, pid);
    }
    else{
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }

    if(job == NULL){
        printf("No such job\n");
        return;
    }

    Kill(-(job->pid), SIGCONT);
    if(!strcmp(argv[0], "bg")){
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid,job->cmdline);
    }else{
        job->state = FG;
        waitfg(job->pid);
    }

    return;
}

/*
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    //TODO suspend version
    while(pid == fgpid(jobs)){
        sleep(0);
    }
    return;
}

/*
 * history_add - add a command line to the history list
 */
int history_add(const char *cmdline)
{
    char *linecopy;
    /* Initialize */
    if(history == NULL){
        history = malloc(sizeof(char *) * MAXHIST);
        if(history == NULL){
            Sio_puts("Add history failed");
            return 1;
        }
        memset(history, 0, sizeof(char *) * MAXHIST);
    }

    /* Add an heap allocated copy of the line in the history.
     * If we reached the max length, remove the older line. */
    linecopy = strdup(cmdline);
    if (!linecopy) return 0;
    if (history_len == MAXHIST) {
        free(history[0]);
        memmove(history,history+1,sizeof(char*)*(MAXHIST-1));
        history_len--;
    }
    history[history_len] = linecopy;
    history_len++;
    return 0;
}

/*
 * Free the history, but does not reset it. Only used when we have to
 * exit() to avoid memory leaks
 */
void history_free(void)
{
    if (history) {
        for (int j = 0; j < history_len; j++)
            free(history[j]);
        free(history);
    }
}

/*
 * print all history command lines
 */
void history_list(void)
{
    if (history){
        for (int i = 0; i < history_len; i++)
            printf("%s\n", history[i]);
    }
}

/*****************
 * Signal handlers
 *****************/
/*
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.
 */
void sigchld_handler(int sig)
{
    int olderrno = errno;
    sigset_t mask_all, prev_all;
    int status;
    pid_t pid;

    Sigfillset(&mask_all);
    while((pid_flag = pid = Waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) /* Reap a zombie child */
    {
        if(WIFEXITED(status)){  /*process is exited in normal way*/
            Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            deletejob(jobs, pid);
            Sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }
        if(WIFSIGNALED(status)){/*process is terminated by a signal*/
            printf("Job [%d] (%d) terminated by signal %d\n",pid2jid(pid),pid,WTERMSIG(status));
            Sigprocmask(SIG_BLOCK, &mask_all, &prev_all);
            deletejob(jobs, pid);
            Sigprocmask(SIG_SETMASK, &prev_all, NULL);
        }
        if(WIFSTOPPED(status)){/*process is stop because of a signal*/
            printf("Job [%d] (%d) stopped by signal %d\n",pid2jid(pid),pid,WSTOPSIG(status));
            struct job_t *job = getjobpid(jobs,pid);
            if(job !=NULL )
                job->state = ST;
        }
        Sio_puts("sigchld_handler\n");
    }

//    if(errno != ECHILD){
//        unix_error("waitpid error");
////        Sio_error("Fuck waitpid error\n");
//    }

    errno = olderrno;
}

/*
 * sigint_handler - The kernel sends a SIGINT to the shell whenever the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.
 */
void sigint_handler(int sig)
{
    pid_t pid = fgpid(jobs);
    if(pid != 0)
        Kill(-pid, SIGINT);
    Sio_puts("sigint_handler\n");
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.
 */
void sigtstp_handler(int sig)
{
    pid_t pid =fgpid(jobs);
    if(pid!=0 ){
        struct job_t *job = getjobpid(jobs,pid);
        if(job->state == ST){  /*already stop the job ,do‘t do it again*/
            return;
        }else{
            Kill(-pid,SIGTSTP);
        }
    }
    Sio_puts("sigtstp_handler\n");
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig)
{
    history_free();
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}
/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
//    for(int i = 0; i < MAXPIDS; i++)
//        job->pid[i] = 0;
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs)
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid > max)
            max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline)
{
    int i;

    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        for(int j = 0; j < MAXPIDS; j++)
        {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = nextjid++;
            if (nextjid > MAXJOBS)
                nextjid = 1;
            strcpy(jobs[i].cmdline, cmdline);
            if(verbose){
                printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;

        }

    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid)
{
    int i;
    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            clearjob(&jobs[i]);
            nextjid = maxjid(jobs)+1;
            return 1;
        }
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].state == FG)
            return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
            return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid)
{
    int i;

    if (jid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid == jid)
            return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid)
{
    int i;

    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */
void listjobs(struct job_t *jobs)
{
    int i;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid != 0) {
            printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
            switch (jobs[i].state) {
                case BG:
                    printf("Running ");
                    break;
                case FG:
                    printf("Foreground ");
                    break;
                case ST:
                    printf("Stopped ");
                    break;
                default:
                    printf("listjobs: Internal error: job[%d].state=%d ",
                           i, jobs[i].state);
            }
            printf("%s", jobs[i].cmdline);
        }
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void)
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler)
{
    struct sigaction action, old_action;

    action.sa_handler = handler;
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}


void Sigfillset(sigset_t *set)
{
    if(sigfillset(set) < 0)
        unix_error("Sigfillset error\n");
}

void Sigprocmask(int how, const sigset_t *set, sigset_t *oldset)
{
    if(sigprocmask(how, set, oldset) < 0)
        unix_error("Sigprocmask error\n");
    return;
}

void Sigemptyset(sigset_t *set)
{
    if(sigemptyset(set) < 0)
        unix_error("Sigemptyset error\n");
    return;
}

void Sigaddset(sigset_t *set, int signum)
{
    if(sigaddset(set, signum) < 0)
        unix_error("Sigaddset error\n");
    return;
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

int Pipe(int pipefd[2])
{
    int ret;
    ret = pipe(pipefd);
    if(ret < 0)
        unix_error("Pipe error");
    return ret;
}

void Kill(pid_t pid, int signum)
{
    int rc;

    if ((rc = kill(pid, signum)) < 0)
        unix_error("Kill error");
}

pid_t Waitpid(pid_t pid, int *iptr, int options)
{
    pid_t retpid;

    if((retpid  = waitpid(pid, iptr, options)) < 0)
        unix_error("Waitpid error");
    return(retpid);
}


/*************************************************************
 * The Sio (Signal-safe I/O) package - simple reentrant output
 * functions that are safe for signal handlers.
 *************************************************************/
/* sio_strlen - Return length of string (from K&R) */
static size_t sio_strlen(char s[])
{
    int i = 0;

    while (s[i] != '\0')
        ++i;
    return i;
}

ssize_t sio_puts(char s[]) /* Put string */
{
    return write(STDOUT_FILENO, s, sio_strlen(s));
}

void sio_error(char s[]) /* Put error message and exit */
{
    sio_puts(s);
    _exit(1);
}

/* Wrappers for the SIO routines */

ssize_t Sio_puts(char s[])
{
    ssize_t n;

    if ((n = sio_puts(s)) < 0)
        sio_error("Sio_puts error");
    return n;
}

void Sio_error(char s[])
{
    sio_error(s);
}
