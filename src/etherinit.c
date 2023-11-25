#include "etherinit.h"
#include "etherlog.h"
#include "filehandle.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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

char basepath[] = "./serve/";

int generateResp(char* respbuf, char uri[], char method[]) {
    // atm, assuming that all calls are get calls 
    // printf("Called generateResp\n");
    char ibp[strlen(basepath)+strlen(uri)-1]; strcpy(ibp, basepath);

    char* fbuf = malloc(8192 * sizeof(char));
    memset(fbuf, 0, 8192); // this seems to be necessary unfortunately
    int status = readall(strcat(ibp, uri+1), fbuf);
    char conttype[20];
    getFiletype(conttype, uri);

    switch (status) {
        case 200:
            sprintf(respbuf, "HTTP/1.0 %d OK\r\nServer: ether\r\nContent-type: %s\r\n\r\n%s", status, conttype, fbuf);
            break;
        case 404:
            sprintf(respbuf, "HTTP/1.0 %d Not Found\r\nServer: ether\r\nContent-type: text/html\r\n\r\n<html>404 Not Found. Keep Looking!</html>", status);
            break;
        case 500:
            sprintf(respbuf, "HTTP/1.0 %d Internal Server Error\r\nServer: ether\r\nContent-type: text/html\r\n\r\n<html><h1>500 Internal Server Error</h1></html>", status);    
            break;
    }

    free(fbuf);

    return status;
}

int run_ether_server(ether_config_t config) {
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

        request_ctx_t ctx;
        extract_context(&ctx, newsockfd, &client_addr, &client_addrlen);

        if (ctx.error != 0) {
            // printf("error in ctx return\n");
            continue;
        }

        char reqdata[1024];
        // sprintf(reqdata, "[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
        sprintf(reqdata, "%s | %s | %s\n", ctx.request_ip, ctx.method, ctx.uri);
        // write to socket
        // printf("calling generateResp\n");
        char* _resp = malloc(8192 * sizeof(char));
        int status = generateResp(_resp, ctx.uri, ctx.method);
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
        sprintf(ftime, "%d.%09lds", diff, now.tv_nsec-starttime.tv_nsec);
        handledreq(status, reqdata, ftime);
        
        free(_resp);
        close(newsockfd);
    }
    return 0;
}


void extract_context(request_ctx_t* ctx, int newsockfd, struct sockaddr_in* client_addr, int* client_addrlen) {
    // printf("Called\n");
    char buffer[BUFSIZE];

    // get client details
    int sockn = getsockname(newsockfd, (struct sockaddr *)client_addr, (socklen_t *)client_addrlen);
    if (sockn < 0) {
        perror("ether (getsockname)");
        ctx->error = 1;
        return;
    }
    // printf("Got client details.\n");

    // read from socket
    int valread = read(newsockfd, buffer, BUFSIZE);
    if (valread < 0) {
        perror("ether (read)");
        ctx->error = 1;
        return;
    }

    // printf("Read from socket\n");

    // Read the request
    // char method[BUFSIZE], uri[BUFSIZE], version[BUFSIZE];
    sscanf(buffer, "%s %s %s", ctx->method, ctx->uri, ctx->version);
    if (!strcmp(ctx->uri, "/")) {
        strcpy(ctx->uri, "/main.html");
    }

    // printf("Read request\n");

    sprintf(ctx->request_ip, "%s", inet_ntoa((*client_addr).sin_addr));
}