// Tim Robertson 2018
// keyboard.c
// The sole purpose of keyboard.c is to take input from the terminal
// and add the input to a list of messages to be sent to the remote client.
// The keyboard_routine will run in a separate thread and shares a mutex with the send thread.
// Keyboard and Send share the sendKeyboardList and can only access it when the sendKeyboardMutex is unlocked.
#include "keyboard.h"
#include "list.h"
#include "globals.h"
#include "util.h"

#include <pthread.h>
#include <stdio.h> 
#include <string.h> 
#include <fcntl.h>
#include <unistd.h>

void setNonBlockingInput() {
    int flag = fcntl(1, F_GETFL);
    if (flag == -1) {
        printf("WARN: Unable to get blocking flags for stdin\n");
    }

    flag = fcntl(1, F_SETFL, flag | O_NONBLOCK);
    if (flag == -1) {
        printf("WARN: Unable to set stdin flag to non-blocking. Keyboard thread may block\n");
    }
}

void *keyboard_routine(void *list) {
    setNonBlockingInput();

    // Set list from thread caller
	LIST* sendKeyboardList;
    sendKeyboardList = (LIST*)list;

    while(shutdownSignal == 0) {

        char msgBuffer[MAX_MESSAGE_SIZE];

        if(fgets(msgBuffer, sizeof(msgBuffer), stdin) != NULL) {
            // Search msg for newline and replace with \0 if found
            char *newline = strchr(msgBuffer, '\n');
            if (newline != NULL) {
                *newline = '\0';
            }

            checkForShutdownChar(msgBuffer);

            // Get mutex
            pthread_mutex_lock(&sendKeyboardMutex);

            // Append keyboard message to the list 
            ListAppend(sendKeyboardList, msgBuffer);

            // Signal to send thread that the list has an item
            pthread_cond_signal(&sendKeyboardCond);

            // Unlock mutex
            pthread_mutex_unlock(&sendKeyboardMutex);
        }

        // Sleep for 100ms
        usleep(100000);
    }

    pthread_exit(NULL);
}