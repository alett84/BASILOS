#ifndef MPX_STDLIB_H
#define MPX_STDLIB_H

#include <stdint.h>
/**
 @file stdlib.h
 @brief A subset of standard C library functions.
*/

/**
 Convert an ASCII string to an integer
 @param s A NUL-terminated string
 @return The value of the string converted to an integer
*/
int atoi(const char *s);

/**
 Returns the smaller of two int arguments
 @param a First int arg
 @param b Second int arg
 @return smaller of a and b
 */
int imin(int a, int b);

// R5 MANUAL THIS
/**
 * @brief returns a translated hex string 
 * 
 * @param *hex_str a hex string to be translated 
 * @return uintptr_t 
 */
uintptr_t hex_to_uintptr(const char *hex_str);


#endif
