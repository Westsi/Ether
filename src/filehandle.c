#include <stdlib.h>
#include <stdio.h>

// returns status code, allocs buffer, does not free - caller needs to do that
int readall(char path[], char* buffer) {
    FILE    *infile;
    // char    *buffer;
    long    numbytes;

    printf("Reading from %s\n", path);
    
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
    
    /* copy all the text into the buffer */
    fread(buffer, sizeof(char), numbytes, infile);
    fclose(infile);
    
    /* confirm we have read the file by
    outputing it to the console */
    // printf("The file called test.dat contains this text\n\n%s", buffer);
    
    return 200;
}