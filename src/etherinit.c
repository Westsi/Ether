#include "etherinit.h"
#include <time.h>

ether_config_t init_ether_server() {
    ether_config_t config;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("ether (socket)");
        exit(-1);
    }
    printf("socket created\n");

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    config.sockfd = sockfd;
    config.host_addr = host_addr;
    config.host_addrlen = host_addrlen;
    return config;
}

int run_ether_server(ether_config_t config) {
    char buffer[BUFSIZE];
    // Create client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    // bind socket to address
    if (bind(config.sockfd, (struct sockaddr *)&config.host_addr, config.host_addrlen) != 0) {
        perror("ether (bind)");
        return -1;
    }
    printf("socket bound to address\n");

    if (listen(config.sockfd, SOMAXCONN) != 0) {
        perror("ether (listen)");
        return -1;
    }
    printf("listening for connections...\n");

    for (;;) {
        // accept incoming connections
        int newsockfd = accept(config.sockfd, (struct sockaddr *)&config.host_addr, (socklen_t *)&config.host_addrlen);

        struct timespec starttime;
        timespec_get(&starttime, TIME_UTC);

        if (newsockfd < 0) {
            perror("ether (accept)");
            return 1;
        }
        // printf("connection accepted\n");

        // get client details
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0) {
            perror("ether (getsockname)");
            continue;
        }

        // read from socket
        int valread = read(newsockfd, buffer, BUFSIZE);
        if (valread < 0) {
            perror("ether (read)");
            continue;
        }

        // Read the request
        char method[BUFSIZE], uri[BUFSIZE], version[BUFSIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        if (!strcmp(uri, "/")) {
            strcpy(uri, "/main.html");
        }
        char reqdata[1024];
        // sprintf(reqdata, "[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
        sprintf(reqdata, "%s | %s | %s\n", inet_ntoa(client_addr.sin_addr), method, uri);
        // write to socket
        // printf("calling generateResp\n");
        char* _resp = malloc(8192 * sizeof(char));
        int status = generateResp(_resp, uri, method);
        // printf("writing resp\n");
        int valwrite = write(newsockfd, _resp, strlen(_resp));
        if (valwrite < 0) {
            perror("ether (write)");
            continue;
        }

        // sleep(1);
        struct timespec now;
        timespec_get(&now, TIME_UTC);
        int diff;
        diff = gmtime(&now.tv_sec)->tm_sec - gmtime(&starttime.tv_sec)->tm_sec;
        // diff += now.tv_nsec - starttime.tv_nsec;
        char ftime[80];
        sprintf(&ftime, "%d.%09lds", diff, now.tv_nsec-starttime.tv_nsec);
        handledreq(status, reqdata, ftime);
        
        free(_resp);
        close(newsockfd);
    }
    return 0;
}