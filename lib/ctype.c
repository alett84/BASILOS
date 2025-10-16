#include <ctype.h>

int isspace(int c)
{
	return (c == ' ' || c == '\n' || c == '\r' || c == '\f' || c == '\t' || c == '\v');
}

int isdigit(char c)
{
	return (c >= '0' && c <= '9');
}

char tolower(char c)
{
	if (c >= 'A' && c <= 'Z') 
	{
		c += 'a' - 'A';
	}
	return c;
}

int tolower_inplace(char *c) 
{
	if (*c >= 'A' && *c <= 'Z')
	{
		*c += 'a' - 'A';
		return 1;
	}
	return 0;
}
