#ifndef MPX_CMD_PCB_H
#define MPX_CMD_PCB_H

/**
 @file mpx/cmd_pcb.h
 @brief Command handling functions for PCB functionality
*/

/**
 Command handling for pcb commands
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb(char** toks, int token_count);

/**
 Command to show a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_show(char** toks, int token_count);

/**
 Command to delete a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_delete(char** toks, int token_count);

/**
 Command to block a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_block(char** toks, int token_count);

/**
 Command to unblock a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_unblock(char** toks, int token_count);

/**
 Command to suspend a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_suspend(char** toks, int token_count);

/**
 Command to resume a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_resume(char** toks, int token_count);

/**
 Command to change priority of a pcb
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_priority(char** toks, int token_count);

/**
 Command to show all ready pcbs
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_showready();

/**
 Command to show all running pcbs
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_showrunning();

/**
 Command to show all blocked pcbs
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
 void cmd_pcb_showblocked();

/**
 Command to show all pcbs
 @param toks command arguments recieved from comhand
 @param token_count number of command arguments
 */
void cmd_pcb_showall();

/** Command to yield comhand */
//void cmd_pcb_yield(); 

/** Comand to load r3 processes */
void cmd_pcb_loadr3(); 

#endif
