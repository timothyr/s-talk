// Tim Robertson 2018
// util.c
// Contains a utility function used by most of the threads.
#include "util.h"
#include "globals.h"

#include <string.h> 

void checkForShutdownChar(char *msg) {
    if(strlen(msg) == 1) {
        if(msg[0] == SHUTDOWN_CHAR && msg[1] == '\0') {    
            shutdownSignal = 1;
        }
    }
}

