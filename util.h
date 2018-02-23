#ifndef UTIL_H
#define UTIL_H

void checkForShutdownChar(char *msg);

struct addrinfo *initSocket(int *sockfd, char *hostname, char *port);

#endif
