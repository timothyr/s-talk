#ifndef GLOBALS_H
#define GLOBALS_H

#include <pthread.h>

#define MAX_MESSAGE_SIZE 140

// If this is typed then shutdown the program
#define SHUTDOWN_CHAR '!'

// Global shutdown signal, set to 0 initially and 1 if shutting down
int shutdownSignal;

// Mutual exclusion between send and keyboard threads
pthread_mutex_t sendKeyboardMutex;
pthread_cond_t sendKeyboardCond;

// Mutual exclusion between recv and screen threads
pthread_mutex_t recvScreenMutex;
pthread_cond_t recvScreenCond;

// Sockets for sending and receiving. Send to remote and receive to local
int localSocket;
int remoteSocket;

// The ip address and port info of the remote client
struct addrinfo *remoteAddr;

#endif
