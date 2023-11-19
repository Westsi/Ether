#ifndef ETHER_FILEHANDLE_H_
#define ETHER_FILEHANDLE_H_

typedef enum {
    TEXT,
    HTML,
    CSS,
    JS
} FileType;

int readall(char path[], char* buffer);
FileType getFiletype(char uri[]);
#endif