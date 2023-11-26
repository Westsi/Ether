#include <stdlib.h>
#include <stdio.h>
#include "filehandle.h"
#include "etherlog.h"
#include <string.h>

// returns status code, allocs buffer, does not free - caller needs to do that
int readall(char path[], char* buffer) {
    FILE    *infile;
    // char    *buffer;
    long    numbytes;

    // printf("Reading from %s\n", path);
    
    /* open an existing file for reading */
    infile = fopen(path, "r");
    
    /* quit if the file does not exist */
    if(infile == NULL)
        return 404;
    
    /* Get the number of bytes */
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    
    /* reset the file position indicator to 
    the beginning of the file */
    fseek(infile, 0L, SEEK_SET);
    
    /* grab sufficient memory for the 
    buffer to hold the text */
    // buffer = (char*)calloc(numbytes, sizeof(char));	
    
    /* memory error */
    if(buffer == NULL)
        return 500;
    // printf("last known communication.\n");
    /* copy all the text into the buffer */
    fread(buffer, sizeof(char), numbytes, infile);
    // printf("fread worked\n");
    fclose(infile);
    // printf("fclose returned\n");
    
    /* confirm we have read the file by
    outputing it to the console */
    printf("The file contains this text\n\n%s", buffer);
    
    return 200;
}

mime_map mime_types [] = {
    {"css", "text/css"},
    {"gif", "image/gif"},
    {"htm", "text/html"},
    {"html", "text/html"},
    {"jpeg", "image/jpeg"},
    {"jpg", "image/jpeg"},
    {"ico", "image/x-icon"},
    {"js", "application/javascript"},
    {"pdf", "application/pdf"},
    {"mp4", "video/mp4"},
    {"mp3", "audio/mpeg"},
    {"png", "image/png"},
    {"svg", "image/svg+xml"},
    {"xml", "text/xml"},
    {NULL, NULL},
};

void getFiletype(char conttype[], char uri[]) {
    char* last = strrchr(uri, '.')+1;
    mime_map *map = mime_types;
    while (map->extension) {
        if (strcmp(map->extension, last) == 0) {
            strcpy(conttype, map->mime_type);
            return;
        }
        map++;
    }
    strcpy(conttype, "text/plain");

}