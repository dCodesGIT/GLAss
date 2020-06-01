// To demonstrate BREAK statement - using multiple loops
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	for(i = 1; i <= 20; i++) {
		for(j = 1; j <= 20; j++) {
			if(j > i)
				break;
			else
				printf(" *");
		}
		printf("\n");
	}

	printf("\n\n");
	return 0;
}