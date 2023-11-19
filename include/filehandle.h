#ifndef ETHER_FILEHANDLE_H_
#define ETHER_FILEHANDLE_H_

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map;

int readall(char path[], char* buffer);
void getFiletype(char conttype[], char uri[]);
#endif