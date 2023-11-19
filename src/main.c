#include <stdio.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "ethererror.h"
#include "filehandle.h"
#include "etherlog.h"
#include "structs.h"
#include "etherinit.h"

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

int main(void) {
    // char resp[] = "HTTP/1.0 200 OK\r\n"
    //               "Server: ether\r\n"
    //               "Content-type: text/html\r\n\r\n"
    //               "<html>hello, world</html>\r\n";
    
    ether_config_t config = init_ether_server();

    int rval = run_ether_server(config);
    return rval;
}