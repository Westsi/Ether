#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "cweberror.h"
#include "filehandle.h"

#define PORT 8080
#define BUFSIZE 1024

char basepath[] = "./serve/";

void generateResp(char* respbuf, char uri[], char method[]) {
    // atm, assuming that all calls are get calls 
    printf("Called generateResp\n");   
    char ibp[strlen(basepath)+strlen(uri)-1]; strcpy(ibp, basepath);

    char* fbuf = malloc(8192 * sizeof(char));
    int status = readall(strcat(ibp, uri+1), fbuf);
    char conttype[20];
    FileType ft = getFiletype(uri);
    switch (ft) {
        case TEXT:
            strcpy(conttype, "text/plain");
            break;
        case HTML:
            strcpy(conttype, "text/html");
            break;
        case CSS:
            strcpy(conttype, "text/css");
            break;
        case JS:
            strcpy(conttype, "text/javascript");
            break;
    }

    switch (status) {
        case 404:
            sprintf(respbuf, "HTTP/1.0 %d Not Found\r\nServer: cweb\r\nContent-type: %s\r\n\r\n<html>404 Not Found. Keep Looking!</html>", status, conttype);
        case 500:
            sprintf(respbuf, "HTTP/1.0 %d Internal Server Error\r\nServer: cweb\r\nContent-type: %s\r\n\r\n<html><h1>500 Internal Server Error</h1></html>", status, conttype);    
    }

    sprintf(respbuf, "HTTP/1.0 %d OK\r\nServer: cweb\r\nContent-type: %s\r\n\r\n%s", status, conttype, fbuf);
    printf("Resp Status = %d\n", status);
}

int main(void) {
    char buffer[BUFSIZE];
    char resp[] = "HTTP/1.0 200 OK\r\n"
                  "Server: cweb\r\n"
                  "Content-type: text/html\r\n\r\n"
                  "<html>hello, world</html>\r\n";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("cweb (socket)");
        return -1;
    }
    printf("socket created\n");

    struct sockaddr_in host_addr;
    int host_addrlen = sizeof(host_addr);
    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Create client address
    struct sockaddr_in client_addr;
    int client_addrlen = sizeof(client_addr);

    // bind socket to address
    if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0) {
        perror("cweb (bind)");
        return -1;
    }
    printf("socket bound to address\n");

    if (listen(sockfd, SOMAXCONN) != 0) {
        perror("cweb (listen)");
        return -1;
    }
    printf("listening for connections...\n");

    for (;;) {
        // accept incoming connections
        int newsockfd = accept(sockfd, (struct sockaddr *)&host_addr, (socklen_t *)&host_addrlen);
        if (newsockfd < 0) {
            perror("cweb (accept)");
            return 1;
        }
        printf("connection accepted");

        // get client details
        int sockn = getsockname(newsockfd, (struct sockaddr *)&client_addr, (socklen_t *)&client_addrlen);
        if (sockn < 0) {
            perror("cweb (getsockname)");
            continue;
        }

        // read from socket
        int valread = read(newsockfd, buffer, BUFSIZE);
        if (valread < 0) {
            perror("cweb (read)");
            continue;
        }

        // Read the request
        char method[BUFSIZE], uri[BUFSIZE], version[BUFSIZE];
        sscanf(buffer, "%s %s %s", method, uri, version);
        if (!strcmp(uri, "/")) {
            strcpy(uri, "/main.html");
        }
        printf("[%s:%u] %s %s %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), method, version, uri);
        // write to socket
        printf("calling generateResp\n");
        char* _resp = malloc(8192 * sizeof(char));
        generateResp(_resp, uri, method);
        printf("writing resp\n");
        int valwrite = write(newsockfd, _resp, strlen(_resp));
        if (valwrite < 0) {
            perror("cweb (write)");
            continue;
        }
        free(_resp);
        close(newsockfd);
    }
    return 0;
}