#include <stdio.h>
#include "csapp.h"

#include "cache.h" // cache
#include "sbuf.h"

#define NTHREADS 4
#define SBUFSIZE 16

sbuf_t sbuf;    /* Shared buffer of connected descriptors */

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
// cache
#define LRU_MAGIC_NUMBER 9999
#define CACHE_OBJS_COUNT 10
cache_t cache;  /* Shared cache */

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

void doit(int fd);
void read_requesthdrs(rio_t *rp);
void write_request(int fd, char *method, char *filename, char *hostname, char *port);
int parse_uri(char *uri, char *hostname, char *port, char *filename);

void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);

void sigpipe_handler();
void *thread(void *vargp);

/* Main routine */
int main(int argc, char **argv)
{
    /* Check command line args */
    if (argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    /* CSAPP Page987 Aside */
    Signal(SIGPIPE, sigpipe_handler);

    int listenfd, connfd, *connfdp;
    char hostname[MAXLINE], port[MAXLINE];
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    cache_init(&cache);
    sbuf_init(&sbuf, SBUFSIZE);

    // open a listening socket
    listenfd = Open_listenfd(argv[1]);

    for (int i = 0; i < NTHREADS; i++) {    /* Create worker threads */
        Pthread_create(&tid, NULL, thread, NULL);
    }

    // execute the typical infinite server loop
    while (1) {
        clientlen = sizeof(clientaddr);
        // to avoid the potentially deadly race
        connfdp = Malloc(sizeof(int));
        // repeatedly accept a connection request
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);

        sbuf_insert(&sbuf, *connfdp);

        printf("Accepted connection from (%s, %s)\n", hostname, port);
        // perform a transaction
        // pthread_create(&tid, NULL, thread, connfdp);
    }
    return 0;
}

void* thread(void *vargp){
    // int connfd = *((int *) vargp);
    // Pthread_detach(pthread_self());
    // Free(vargp);
    // doit(connfd);
    // Close(connfd);
    Pthread_detach(pthread_self());
    while (1) {
        int connfd = sbuf_remove(&sbuf);
        doit(connfd);   /* Service client */
        Close(connfd);
    }
}

/* 
 * catach SIGPIPE 
 */
void sigpipe_handler()
{
    Sio_error("SIGPIPE caught!");
}

/*
 * doit - handle one HTTP request/response transaction
 *        only GET is implemented
 */
/* $begin doit */
void doit(int fd) 
{
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], hostname[MAXLINE], port[MAXLINE];
    rio_t rio, rio_server;

    /* Read request line and headers */
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;
    printf("%s", buf);
    /* parse request */
    sscanf(buf, "%s %s %s", method, uri, version);
    // if the client request another method, e.g., POST, 
    // send the client an error message and return to the main routine,
    // which then closes the connection and awaits the next connection request
    if (strcasecmp(method, "GET")) {                     
        clienterror(fd, method, "501", "Not Implemented",
                    "Tiny does not implement this method");
        return;
    }                                                    
    /* tiny does not use any of the information in the request headers. */
    /* simply read and ignore any request headers */
    read_requesthdrs(&rio);                              

    /* find cache */
    int index;
    if ((index = cache_find(&cache, uri)) != -1) {  /* cache hits */
        read_pre(&cache, index);
        Rio_writen(fd, cache.cacheobjs[index].obj, strlen(cache.cacheobjs[index].obj));
        read_after(&cache, index);
        return;
    }

    /* Parse URI from GET request */
    parse_uri(uri, hostname, port, filename);

    // the proxy serves as client now
    int clientfd = Open_clientfd(hostname, port); 
    // send request to server
    write_request(clientfd, method, filename, hostname, port);

    Rio_readnb(&rio, buf, rio.rio_cnt);
    
    Rio_readinitb(&rio_server, clientfd);

    char cache_buf[MAX_OBJECT_SIZE]; // cache
    int buf_size = 0, n;
    // read response from the server and send server response to client
    // why while loop is a must?
    while( (n = Rio_readlineb(&rio_server, buf, MAXLINE)) != 0 ){
        buf_size += n;
        if (buf_size < MAX_OBJECT_SIZE) {
            strcat(cache_buf, buf);
        }
        Rio_writen(rio.rio_fd, buf, strlen(buf));
    }
        

    Close(clientfd);

    /* store it in cache */
    if (buf_size < MAX_OBJECT_SIZE) {
        cache_store(&cache, uri, cache_buf);
    }
}
/* $end doit */

/*
 * read_requesthdrs - read HTTP request headers
 * 
 */
/* $begin read_requesthdrs */
void read_requesthdrs(rio_t *rp) 
{
    char buf[MAXLINE];

    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
    /* the empty text line that terminates the request headers 
     * consists of a carriage return and line feed pair */
    while(strcmp(buf, "\r\n")) {        
        Rio_readlineb(rp, buf, MAXLINE);
        printf("%s", buf);
    }
    return;
}
/* $end read_requesthdrs */

/*
 * parse_uri - parse URI into hostname, port, filename
 */
/* $begin parse_uri */
int parse_uri(char *uri, char *hostname, char *port, char *filename) 
{
    char *ptr;
    // ignore http://
    if( strstr(uri, "http://") ) uri = uri + 7;

    // find the filename
    if (ptr = index(uri, '/')) {
        strcpy(filename, ptr);
        *ptr = '\0';
    } 
    else {
        strcpy(filename, "/index.html"); // default home page
    }

    // find the port
    if (ptr = index(uri, ':')) {
        strcpy(port, ptr+1);
        *ptr='\0';
    }

    // the rest is the hostname
    strcpy(hostname, uri);
    return 0;
}
/* $end parse_uri */

/*
 * write_request - write HTTP request line and headers to the server
 */
/* $begin write_request */
void write_request(int fd, char *method, char *filename, char *hostname, char *port) 
{
    char buf[MAXLINE];

    sprintf(buf, "%s %s HTTP/1.0\r\n", method, filename);
    // user is very likely not to provide port
    if (port[0] != '\0')
        sprintf(buf, "%sHOST: %s:%s\r\n", buf, hostname, port);
    else 
        sprintf(buf, "%sHOST: %s\r\n", buf, hostname);  
    sprintf(buf, "%sConnection: close\r\n", buf);
    sprintf(buf, "%sProxy-Connection: close\r\n", buf);
    sprintf(buf, "%s%s\r\n", buf, user_agent_hdr);
    Rio_writen(fd, buf, strlen(buf));
}
/* $end write_request */

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Tiny Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */

