#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

int atoi(const char *s)
{
	int res = 0;
	char sign = ' ';

	while (isspace(*s)) {
		s++;
	}

	if (*s == '-' || *s == '+') {
		sign = *s;
		s++;
	}

	while ('0' <= *s && *s <= '9') {
		res = res * 10 + (*s - '0');
		s++;

	}

	if (sign == '-') {
		res = res * -1;
	}

	return res;
}

int imin(int a, int b)
{
	if (a < b) 
	{
		return a;
	}
	return b;
}

//New R5

uintptr_t hex_to_uintptr(const char *hex_str)
{
	uintptr_t value = 0;

	//skip the prefix "0x"
	if(hex_str[0] == '0' && (hex_str[1] =='x' || hex_str[1] == 'X'))
	{
		hex_str += 2;
	}

	while(*hex_str)
	{
		char c = *hex_str++; 
		int digit = 0;

		if(c >= '0' && c <= '9')
			digit = c - '0';
		else if (c >= 'a' && c <= 'f')
            digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F')
            digit = c - 'A' + 10;
        else
            break; // Or handle invalid characters appropriately
        
        value = (value << 4) | digit;  // Multiply by 16 and add the digit.
    }
    
    return value;
}
