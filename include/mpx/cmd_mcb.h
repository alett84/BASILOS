#ifndef MPX_CMD_MCB_H
#define MPX_CMD_MCB_H

#include <stdint.h>

// R5 MANUAL THIS

/**
 Goes trhough and checks the tokens to see which function to 
 pass to 
 @param toks array of tokens from command line
 @param token_count number of token in toks array 
 */
void cmd_mcb (char** toks, int token_count);

/**
 Frees memory by passing a address from toks to 
 the free_memory command 
 @param toks array of tokens from command line 
 @param token_count number of token in toks array 
 */
void cmd_mcb_free(char** toks, int token_count);

/**
 Command to allocate an mcb
 @param toks array of tokens from command line 
 @param token_count number of token in toks array 
 */
void cmd_mcb_allocate(char** toks, int token_count);

/**
 Command to show all mcbs of type
 @param allocated MCB_ALLOCATED or MCB_FREE for allocated or free mcbs, -1 for all
 */
void cmd_mcb_show(int allocated);

#endif
