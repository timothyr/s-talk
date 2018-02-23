// Tim Robertson 2018
// screen.c
// The sole purpose of screen.c is to take items off the shared recvScreenList 
// and print the messages to the screen.
// The screen_routine will run in a separate thread and shares a mutex with the recv thread.
// Recv and Screen share the recvScreenList and can only access it when the recvScreenMutex is unlocked.
#include "screen.h"
#include "list.h"
#include "globals.h"
#include "util.h"

#include <pthread.h>
#include <stdio.h>

void *printMessage(char *msg) {
    printf("Received: %s\n", msg);
}

void *screen_routine(void* list) {
    // Set list from thread caller
    LIST* recvScreenList;
    recvScreenList = (LIST*)list;

    while(shutdownSignal == 0) {
        // Get mutex
        pthread_mutex_lock(&recvScreenMutex);
        
        // Wait until signal from recv if the list is empty
        if(ListCount(recvScreenList) == 0) {
            pthread_cond_wait(&recvScreenCond, &recvScreenMutex);
        }
        
        // Print messages from list 
        if(ListCount(recvScreenList) > 0) {
            char *msg;
            msg = (char *)ListTrim(recvScreenList);

            checkForShutdownChar(msg);

            printMessage(msg);
        }

        pthread_mutex_unlock(&recvScreenMutex);
    }

    pthread_exit(NULL);
}
