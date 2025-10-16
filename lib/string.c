#include <string.h>
#include <ctype.h>

/* memcpy() and memset() are in core.c */

int strcmp(const char *s1, const char *s2)
{

	// Remarks:
	// 1) If we made it to the end of both strings (i. e. our pointer points to a
	//    '\0' character), the function will return 0
	// 2) If we didn't make it to the end of both strings, the function will
	//    return the difference of the characters at the first index of
	//    indifference.
	while ((*s1) && (*s1 == *s2)) {
		++s1;
		++s2;
	}
	return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

size_t strlen(const char *s)
{
	size_t len = 0;
	while (*s++) {
		len++;
	}
	return len;
}

char *strtok(char * restrict s1, const char * restrict s2)
{
	static char *tok_tmp = NULL;
	const char *p = s2;

	//new string
	if (s1 != NULL) {
		tok_tmp = s1;
	}
	//old string cont'd
	else {
		if (tok_tmp == NULL) {
			return NULL;
		}
		s1 = tok_tmp;
	}

	//skip leading s2 characters
	while (*p && *s1) {
		if (*s1 == *p) {
			++s1;
			p = s2;
			continue;
		}
		++p;
	}

	//no more to parse
	if (!*s1) {
		return (tok_tmp = NULL);
	}
	//skip non-s2 characters
	tok_tmp = s1;
	while (*tok_tmp) {
		p = s2;
		while (*p) {
			if (*tok_tmp == *p++) {
				*tok_tmp++ = '\0';
				return s1;
			}
		}
		++tok_tmp;
	}

	//end of string
	tok_tmp = NULL;
	return s1;
}

int strtolower(char* s) 
{
	int chars_tolowered = 0;
	for (; *s; s++) 
	{
		chars_tolowered += tolower_inplace(s);
	}
	return chars_tolowered;
}

int itoa(int value, char *buf, int width, int base)
{
	// TODO: does not handle signs properly yet
	int deno = 1;
	for (int i = width - 1; i >= 0; i--)
	{
		buf[i] = '0' + ((value / (deno)) % base);
		if (buf[i] > '9') 
		{
			buf[i] += 'A' - ':';
		}
		deno *= base;
	}
	return value / (deno);
}

int get_i_width(int value, int base)
{
	if (value == 0)
	{
		return 1;
	}

	int width = 0;
	while (value > 0)
	{
		value /= base;
		width++;
	}
	return width;
}

const char *strchr(const char* str, char chr)
{
	for (const char* a = str; a[0]; a++)
	{
		if (a[0] == chr) 
		{
			return a;
		}
	}
	return NULL;
}

const char *strpbrk(const char* str, const char* key)
{
	for (const char* a = str; a[0]; a++)
	{
		for (const char* b = key; b[0]; b++)
		{
			if (a[0] == b[0])
			{
				return a;
			}
		}
	}
	return NULL;
}

int isnumber(const char* str)
{
	if (str[0] == '-' || str[0] == '+')
	{
		str++;
	}
	return isdigit(str[0]);
}


//NEW R3
char* strcpy(char* dest, const char* src) 
{
    char* saved = dest;   
    
    while ((*dest++ = *src++) != '\0') { 
    }
    return saved;
}

//new R4
char *strcat(char *dest, const char *src)
{
    // Find the end of dest
    char *dptr = dest;
    while (*dptr != '\0') {
        dptr++;
    }
    // Copy each character from src to the end of dest
    while (*src != '\0') {
        *dptr = *src;
        dptr++;
        src++;
    }
    // Null-terminate
    *dptr = '\0';
    return dest;
}



