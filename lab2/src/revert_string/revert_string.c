#include "revert_string.h"
#include <string.h>

void RevertString(char *str)
{
	char *start = str;
	char *end = str + strlen(str) - 1;
	
	while (start < end) {
		char x = *start;
		*start = *end;
		*end = x;

		start++;
		end--;
	}
}

