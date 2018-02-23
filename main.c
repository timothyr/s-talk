// Tim Robertson 2018
// s-talk [my port number] [remote machine name] [remote port number]
// A simple server-client chatting program
// Listens on LAN using [my port number] and displays and UDP packets sent to the client
// Sends input from the terminal to [remote machine name] at [remote port number]
// main.c
#include "list.h"
#include "globals.h"
#include "util.h"
#include "keyboard.h"
#include "send.h"
#include "recv.h"
#include "screen.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

pthread_t startThread(pthread_attr_t *attr, void *(* start_routine)(void *), void* list) {
	pthread_t thread;
	int threadError;

	threadError = pthread_create(&thread, attr, start_routine, (void *)list);

	if(threadError) {
		printf("ERROR: pthread_create() returned code %d\n", threadError);
		exit(-1);
	}

	return thread;
}

pthread_attr_t makeJoinableAttr() {
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	return attr;
}

struct addrinfo *initSocket(int *sockfd, char *hostname, char *port) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; 		// IP V4
	hints.ai_socktype = SOCK_DGRAM;	// UDP

	// If only listening then let getaddrinfo fill in our IP for us
	if(hostname == NULL) {
		hints.ai_flags = AI_PASSIVE; // Use my IP
	}

	struct addrinfo *servinfo;
	int addrErr;

	// Get address info on the specific port of our IP
	addrErr = getaddrinfo(hostname, port, &hints, &servinfo);
	
	if(addrErr != 0) {
		printf("ERROR: hostname \"%s\": %s\n", hostname, gai_strerror(addrErr));
		exit(-1);
	}

	// Loop through all available addresses trying to bind sockfd
	struct addrinfo *addr;
	for(addr = servinfo; addr != NULL; addr = addr->ai_next) {
		// Get socket info
		*sockfd = socket(addr->ai_family,
						addr->ai_socktype,
						addr->ai_protocol);
		
		// Socket is not available, continue to next
		if(*sockfd == -1) {
			continue;
		}

		// Allow port to be reused
		//int yes = 1;
		//setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		// Only bind on local sockets for listening, hostname indicates a remote socket
		if(hostname != NULL) {
			break;
		}

		// Bind to socket
		int bindErr;
		bindErr = bind(*sockfd, addr->ai_addr, addr->ai_addrlen);
		
		// Socket is in use, continue to next
		if(bindErr == -1) {
			close(*sockfd);
			continue;
		}

		break;
	}

	// If no sockets were available / all in use
	if(addr == NULL) {
		if(hostname == NULL) {
			printf("ERROR: Failed to bind to socket on port %s\n", port);
		
		} else {
			printf("ERROR: Failed to find socket for sending to port %s\n", port);
		}
		
		exit(-1);
	}

	// Clean up
	freeaddrinfo(servinfo);

	return addr;
}

void listFreeRoutine(void* itemToBeFreed) {
	return;
}

int main (int argc, char *argv[])
{
	if(argc != 4) {
		printf("usage: s-talk [my port number] [remote machine name] [remote port number]\n");
		exit(1);
	}

	// Handle arguments
	char *localPort = argv[1];
	char *remoteHostname = argv[2];
	char *remotePort = argv[3];

	// Set up sockets
	initSocket(&localSocket, NULL, localPort);
	remoteAddr = initSocket(&remoteSocket, remoteHostname, remotePort);

	// Print to user the server info
	char localHostname[50];
	if(gethostname(localHostname, 50) != 0) {
		printf("Listening on port %s\n", localPort);
	} else {
		printf("Listening from %s on port %s\n", localHostname, localPort);
	}

	printf("Enter %c to exit s-talk\n", SHUTDOWN_CHAR);

	shutdownSignal = 0;

	// Initialize mutex and the cond used for signalling
	pthread_mutex_init(&sendKeyboardMutex, NULL);
	pthread_cond_init(&sendKeyboardCond, NULL);

	pthread_attr_t joinableAttr = makeJoinableAttr();

	// The list that is shared between the send and keyboard threads
	LIST* sendKeyboardList = ListCreate();

	// Start send and keyboard threads
	pthread_t keyboardThread = startThread(&joinableAttr, keyboard_routine, (void *)sendKeyboardList);
	pthread_t sendThread = startThread(&joinableAttr, send_routine, (void *)sendKeyboardList);

	// The list that is shared between the recv and screen threads
	LIST* recvScreenList = ListCreate();

	// Start recv and screen threads
	pthread_t recvThread = startThread(&joinableAttr, recv_routine, (void *)recvScreenList);
	pthread_t screenThread = startThread(&joinableAttr, screen_routine, (void *)recvScreenList);

	// Wait for all threads to terminate
	// NOTE: keyboardThread is non-blocking and will reliably exit if shutdown=1
	// Therefore if keyboardThread exits and joins, then the rest can be cancelled
	pthread_join(keyboardThread, NULL);

	shutdownSignal = 1;

	// Broadcast to the conditions to wake up for cancellation
	pthread_cond_broadcast(&sendKeyboardCond);
	pthread_cond_broadcast(&recvScreenCond);

	pthread_cancel(recvThread);
	pthread_cancel(sendThread);
	pthread_cancel(screenThread);

	pthread_join(recvThread, NULL);
	pthread_join(sendThread, NULL);
	pthread_join(screenThread, NULL);

	printf("Exiting s-talk\n");

	// Clean up
	close(remoteSocket);
	close(localSocket);

	ListFree(sendKeyboardList, listFreeRoutine);
	ListFree(recvScreenList, listFreeRoutine);

	pthread_attr_destroy(&joinableAttr);

	pthread_exit(NULL);
}

