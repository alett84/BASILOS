#ifndef MPX_COMHAND_H
#define MPX_COMHAND_H

/**
 @file mpx/comhand.h
 @brief User functions defining the OS's command-line interface
*/

/** Starts the OS's command-line interface */
void comhand(void);

/**
 Command to shutdown the system
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 @return 0 if shutdown is canceled, 1 if shutdown is confirmed
 */
int cmd_shutdown(char** toks, char* buf);

/**
 Command to display help information
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_help(char** toks);

#endif
