// To demonstrate nested FOR loop - Checkers board
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j, c;

	// Code
	for(i = 0;i < 64; i++) {
		for(j = 0; j < 64; j++) {
			c = ((i & 0x8) == 0) ^ ((j & 0x8) == 0);
			if(c == 0)
				printf("  ");
			if(c == 1)
				printf("* ");
		}
		printf("\n");
	}

	printf("\n\n");
	return 0;
}