#ifndef A3_SIO_H
#define A3_SIO_H

#include <unistd.h> //ssize_t, write(), _exit()

ssize_t sio_puts(char s[]);
ssize_t sio_putl(long v);
void sio_error(char s[]);
ssize_t Sio_putl(long v);
ssize_t Sio_puts(char s[]);
void Sio_error(char s[]);

#endif //A3_SIO_H
