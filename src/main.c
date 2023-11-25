#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "ethererror.h"
#include "filehandle.h"
#include "etherlog.h"
#include "structs.h"
#include "etherinit.h"


int main(void) { 
    ether_config_t config = init_ether_server();

    int rval = run_ether_server(config);
    return rval;
}