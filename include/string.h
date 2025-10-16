#ifndef MPX_STRING_H
#define MPX_STRING_H

#include <stddef.h>


#define UP      "\x1B[A"
#define DOWN    "\x1B[B"
#define RIGHT   "\x1B[C"
#define LEFT    "\x1B[D"

#define DELETE  "\x1B[3~"

#define TEXT_CLEAR  "\x1B[0m"
#define TEXT_BLACK  "\x1B[90m"
#define TEXT_RED    "\x1B[91m"
#define TEXT_GREEN  "\x1B[92m"
#define TEXT_YELLOW "\x1B[93m"
#define TEXT_BLUE   "\x1B[94m"
#define TEXT_MAGENTA "\x1B[95m"
#define TEXT_CYAN   "\x1B[96m"
#define TEXT_WHITE  "\x1B[97m"

#define TEXT_DARKGREEN "\x1B[32m"
#define TEXT_DARKCYAN  "\x1B[36m"

#define TEXT_ORANGE "\x1B[38;5;202m"
#define TEXT_GREY "\x1B[38;5;244m"

#define CLEAR "\x1B[2J"
#define HOME  "\x1B[H"

/**
 @file string.h
 @brief A subset of standard C library functions.
*/

/**
 Copy a region of memory.
 @param dst The destination memory region
 @param src The source memory region
 @param n The number of bytes to copy
 @return A pointer to the destination memory region
*/
void* memcpy(void * restrict dst, const void * restrict src, size_t n);

/**
 Fill a region of memory.
 @param address The start of the memory region
 @param c The byte to fill memory with
 @param n The number of bytes to fill
 @return A pointer to the filled memory region
*/
void* memset(void *address, int c, size_t n);

/**
 Compares two strings
 @param s1 The first string to compare
 @param s2 The second string to compare
 @return 0 if strings are equal, <0 if s1 is lexicographically before s2, >0 otherwise
*/
int strcmp(const char *s1, const char *s2);

/**
 Returns the length of a string.
 @param s A NUL-terminated string
 @return The number of bytes in the string (not counting NUL terminator)
*/
size_t strlen(const char *s);

/**
 Split string into tokens
 TODO
*/
char* strtok(char * restrict s1, const char * restrict s2);

/**
 Converts all ASCII uppercase characters to lowercase characters in a given string.
 @param s String to convert to lowercase
 @return The number of characters converted to lowercase
 */
int strtolower(char* s);

/**
 Converts an integer to a n-digit wide string in base y. Is not nul-terminated by function
 @param value integer value to convert to string
 @param buf buffer to store string at
 @param width width of output string
 @param base base of output string. Result will include garbage ASCII values as digits if made higher than 36
 @return integer value of unprinted digits
 */
int itoa(int value, char *buf, int width, int base);

/**
 Gets how many characters would be needed to store an integer value if converted to a string.
 @param value integer value to evaluate
 @param base base to convert integer into
 @return number of characters needed to store value as a string in base base
 */
int get_i_width(int value, int base);

/**
 Finds first instance of a character in a NUL-terminated string
 @param str string to find the character in
 @param chr character to find in string
 @return Pointer to character in string, or NULL if character not found
 */
const char *strchr(const char* str, char chr);

/**
 Finds first instance of any of a given set of characters in a NUL-terminated string
 @param str string to find the characters in
 @param chr characters to find in string
 @return Pointer to character in string, or NULL if character not found
 */
const char *strpbrk(const char* str, const char* key);

/**
 Gets if a position in a string is a number. (not necessarily NUL-terminated)
 @param str string to check
 @return 1 if is number, 0 otherwise
 */
int isnumber(const char* str);

/**
 Copies a NUL-terminated string to another memory location.
 @param dest location to copy into
 @param src location to copy from
 @return dest
 */
char* strcpy(char* dest, const char* src);


//R4
/**
 * A simple reimplementation of strcat that appends `src` to the end of `dest`.
 * Assumes dest has enough space and both strings are null-terminated.
 */
char *strcat(char *dest, const char *src);




#endif
