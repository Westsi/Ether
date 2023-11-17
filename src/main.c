#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "cweberror.h"
#include "filehandle.h"
#include "cweblog.h"
#include <time.h>

#define PORT 8080
#define BUFSIZE 1024

char basepath[] = "./serve/";

int generateResp(char* respbuf, char uri[], char method[]) {
    // atm, assuming that all calls are get calls 
    // printf("Called generateResp\n");   
    char ibp[strlen(basepath)+strlen(uri)-1]; strcpy(ibp, basepath);

    char* fbuf = malloc(8192 * sizeof(char));
    memset(fbuf, 0, 8192); // this seems to be necessary unfortunately
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
        case 200:
            sprintf(respbuf, "HTTP/1.0 %d OK\r\nServer: cweb\r\nContent-type: %s\r\n\r\n%s", status, conttype, fbuf);
            break;
        case 404:
            sprintf(respbuf, "HTTP/1.0 %d Not Found\r\nServer: cweb\r\nContent-type: text/html\r\n\r\n<html>404 Not Found. Keep Looking!</html>", status);
            break;
        case 500:
            sprintf(respbuf, "HTTP/1.0 %d Internal Server Error\r\nServer: cweb\r\nContent-type: text/html\r\n\r\n<html><h1>500 Internal Server Error</h1></html>", status);    
            break;
    }

    free(fbuf);

    return status;
}

int main(void) {
    char buffer[BUFSIZE];
    // char resp[] = "HTTP/1.0 200 OK\r\n"
    //               "Server: cweb\r\n"
    //               "Content-type: text/html\r\n\r\n"
    //               "<html>hello, world</html>\r\n";
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

        struct timespec starttime;
        timespec_get(&starttime, TIME_UTC);

        if (newsockfd < 0) {
            perror("cweb (accept)");
            return 1;
        }
        // printf("connection accepted\n");

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
            perror("cweb (write)");
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