#ifndef LAB0_TINYWEB_H
#define LAB0_TINYWEB_H

#include <netinet/in.h>
#include <stdbool.h>

#ifndef DEFAULT_PORT
#define DEFAULT_PORT 9999
#endif

extern int webfd;
extern bool noise;

/* Simplifies calls to bind(), connect(), and accept() */
typedef struct sockaddr SA;

int open_listenfd(int port);

char *process(int fd, struct sockaddr_in *clientaddr);

#endif /* LAB0_TINYWEB_H */
