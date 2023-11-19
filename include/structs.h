#ifndef ETHER_STRUCTS_H_
#define ETHER_STRUCTS_H_
#include <arpa/inet.h>


typedef struct EtherRequestContext {

} request_ctx_t;

typedef struct EtherRouteInfo {
    char uri[1024];
    char method[1024];
} route_info_t;

typedef void (*handler_function_t)(request_ctx_t*);

// map uri+method to handler function w/ hashmap (route field is key, func is val)
typedef struct Handler {
    handler_function_t func;
    route_info_t route;
} handler_t;

typedef struct EtherConfig {
    int sockfd;
    struct sockaddr_in host_addr;
    int host_addrlen;
    handler_t handlers[1024];
} ether_config_t;

#endif