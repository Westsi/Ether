#ifndef ETHER_INIT_H_
#define ETHER_INIT_H_

#define PORT 8080
#define BUFSIZE 1024

#include "structs.h"

ether_config_t init_ether_server();
int run_ether_server(ether_config_t config);

#endif