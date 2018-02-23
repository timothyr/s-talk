// Tim Robertson 2018
// recv.c
// The sole purpose of recv.c is to add messages received from the remote client to the shared recvScreenList. 
// The recv_routine will run in a separate thread and shares a mutex with the screen thread.
// Recv and Screen share the recvScreenList and can only access it when the recvScreenMutex is unlocked.
#include "recv.h"
#include "list.h"
#include "globals.h"
#include "util.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void *recv_routine(void* recvScreenList) {
    // Set list from thread caller
    LIST* list;
    list = (LIST*)recvScreenList;

    while(shutdownSignal == 0) {

        struct sockaddr_storage remoteAddr;
        socklen_t addr_len;

        char msgBuffer[MAX_MESSAGE_SIZE];

        // Get the message from remote client
        addr_len = sizeof remoteAddr;
        int numBytesReceived;
        numBytesReceived = recvfrom(localSocket, msgBuffer, MAX_MESSAGE_SIZE-1, 0,
                                    (struct sockaddr*)&remoteAddr, &addr_len);
        
        // Don't add invalid message to the list
        if(numBytesReceived == -1) {
            continue;
        }

        // Ensure the data is a valid string
        msgBuffer[numBytesReceived] = '\0';

        checkForShutdownChar(msgBuffer);

        // Get mutex
        pthread_mutex_lock(&recvScreenMutex);

        // Append received message to the list 
        ListAppend(list, msgBuffer);

        // Signal to screen thread that the list has an item
        pthread_cond_signal(&recvScreenCond);

        // Unlock mutex
        pthread_mutex_unlock(&recvScreenMutex);
    }

    pthread_exit(NULL);
}