#ifndef MPX_CMD_DATETIME_H
#define MPX_CMD_DATETIME_H

/**
 @file mpx/cmd_datetime.h
 @brief Command handling functions for RTC date and time functionality
*/

/**
 Command handling for time commands
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_time(char** toks, int token_count);

/** Command to get system time */
void cmd_time_get();

/**
 Command to set system time
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_time_set(char** toks, int token_count);

/**
 Command handling for date commands
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_date(char** toks, int token_count);

/** Command to get system date */
void cmd_date_get();

/**
 Command to set system date
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_date_set(char** toks, int token_count);

#endif
