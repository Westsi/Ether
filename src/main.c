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

void handle_req(request_ctx_t* test) {
    printf("Served main.html.\n");
}


int main(void) { 
    // see for mp3: https://stackoverflow.com/questions/30582473/send-mp3-file-over-socket-in-c
    ether_config_t config = init_ether_server();

    hashmap_set(config.handlers, &(handler_t){.func=handle_req, .route={.method="GET", .uri="/main.html", .hashkey="GET/main.html"}});

    int rval = run_ether_server(config);
    hashmap_free(config.handlers);
    return rval;
}