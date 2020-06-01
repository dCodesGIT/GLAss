// To demonstrate nested DO WHILE loop - Checkers board
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j, c;

	// Code
	i = 0;
	do {
		j = 0;
		do {
			c = ((i & 0x8) == 0) ^ ((j & 0x8) == 0);
			if(c == 0)
				printf("  ");
			if(c == 1)
				printf(" *");
			j++;
		} while(j < 64);
		i++;
		printf("\n");
	} while(i < 64);

	printf("\n\n");
	return 0;
}