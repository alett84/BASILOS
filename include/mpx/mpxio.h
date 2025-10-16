#ifndef MPX_MPXIO_H
#define MPX_MPXIO_H

#include <stddef.h>
/**
 @file mpx/mpxio.h
 @brief Functions to read and write to the command line interface
*/

/**
 Prints a NUL-terminated string to the display console.
 @param str string to print
 */
void print(const char* str);

/**
 Prints a formatted NUL-terminated string to the display console.
 Format fields can be specified by a % sign, and include:

 -  %% - escape for a normal % sign

 -  %s - a string 

 -  %d - an integer in base-10d. Can include a digit count (e.g. %02d)

 -  %x - an integer in base-16d, with hexdigits A~F being lowercase. Can also include a digit count

 -  %X - an integer in base-16d, with hexdigits a~f being uppercase. Can also include a digit count
 
 @param ... stuff to put into format fields, in the order the format fields appear in the string
 */
void printf(const char* str, ...);

/**
 Reads user input from the console, splitting the input into tokens for
 command processing.
 @param toks string array to store tokens into
 @param token_size length of toks array
 @param buf buffer to store user input, will be tokenized
 @param buffer_size length of buffer
 @return number of tokens (or -1 on failed read)
 */
int input(char** toks, int token_size, char* buf, int buffer_size);

int mpx_poll(char *buffer, size_t len);



#endif
