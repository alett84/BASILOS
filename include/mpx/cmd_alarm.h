#ifndef CMD_ALARM_H
#define CMD_ALARM_H

/**
 * Command to create/spawn an Alarm process
 * @param toks The tokenized user input ["alarm", "HH:MM:SS", "message"]
 * @param token_count The number of tokens
 */
void cmd_alarm(char **toks, int token_count);

#endif
