#include <stdarg.h>
#include <string.h>
#include "cweblog.h"
#include <stdio.h>
#include <time.h>

void handledreq(int status, char requestinfo[], char timediff[]) {

    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char ftime[80];
    strftime(ftime,80,"%Y/%m/%d - %H:%M:%S", timeinfo);

    if (status == 200) {
        // time - status - handling time | origin IP | method | path
        printf("[CWEB] %s |\033[1;37;42m %d \033[0m| %s | %s", ftime, status, timediff, requestinfo); // highlight green, reset
    } else {
        printf("[CWEB] %s |\033[1;37;41m %d \033[0m| %s | %s", ftime, status, timediff, requestinfo); // highlight red, reset
    }
}