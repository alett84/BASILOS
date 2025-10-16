#ifndef MPX_CTYPE_H
#define MPX_CTYPE_H

/**
 @file ctype.h
 @brief A subset of standard C library functions.
*/

#define WHITESPACE " \n\r\f\t\v"

/**
 Determine if a character is whitespace.
 @param c Character to check
 @return Non-zero if space, 0 if not space
*/
int isspace(int c);

/**
 Determine if a character is a digit
 @param c Character to ckeck
 @return Non-zero if digit, 0 if not space
*/
int isdigit(char c);

/**
 Convert any ASCII uppercase letters to lowercase letters.
 @param c Character to convert
 @return Character as lowercase if was uppercase, otherwise unchanged
 */
char tolower(char c);

/**
 Convert any ASCII uppercase letters to lowercase letters in place.
 @param c Pointer to character to convert
 @return 1 if the character was uppercase turned lowercase, 0 otherwise
 */
int tolower_inplace(char *c);

#endif
