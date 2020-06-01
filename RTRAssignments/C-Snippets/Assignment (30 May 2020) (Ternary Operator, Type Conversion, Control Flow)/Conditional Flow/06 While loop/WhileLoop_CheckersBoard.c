// To demonstrate WHILE loop - Checkers board
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declartion
	int i, j, c;

	// Code
	i = 0;
	while(i < 64) {
		j = 0;
		while(j < 64) {
			c = ((i & 0x8) == 0) ^ ((j & 0x8) == 0);
			if(c == 0)
				printf("  ");
			if(c == 1)
				printf("* ");
			j++;
		}
		printf("\n");
		i++;
	}

	printf("\n\n");
	return 0;
}