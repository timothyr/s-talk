// Tim Robertson 2018
// send.c
// The sole purpose of send.c is to take items off the shared sendKeyboardList
// and send them to the remote client.
// The send_routine will run in a separate thread and shares a mutex with the keyboard thread.
// Keyboard and Send share the sendKeyboardList and can only access it when the sendKeyboardMutex is unlocked.
#include "send.h"
#include "list.h"
#include "globals.h"
#include "util.h"

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

void *sendOverNetwork(char *msg) {

    int numBytesSent;
    numBytesSent = sendto(remoteSocket, msg, strlen(msg), 0, remoteAddr->ai_addr, remoteAddr->ai_addrlen);

    if(numBytesSent == -1) {
        //printf("ERROR: sendto() could not send %s", msg);
    }
}

void *send_routine(void *list) {
    // Set list from thread caller
    LIST* sendKeyboardList;
    sendKeyboardList = (LIST*)list;

    while(shutdownSignal == 0) {

        // Get mutex
        pthread_mutex_lock(&sendKeyboardMutex);
        
        // Wait until signal from keyboard if the list is empty
        if(ListCount(sendKeyboardList) == 0) {
            pthread_cond_wait(&sendKeyboardCond, &sendKeyboardMutex);
        }
        
        // Send messages from list 
        if(ListCount(sendKeyboardList) > 0) {
            char *msg;
            msg = (char *)ListTrim(sendKeyboardList);

            checkForShutdownChar(msg);

            sendOverNetwork(msg);
        }

        pthread_mutex_unlock(&sendKeyboardMutex);
    }

    pthread_exit(NULL);
}
