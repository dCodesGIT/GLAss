// To demonstrate CONTINUE statement - using multiple loops
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	printf("\n The outer loop prints odd numbers between 1 to 10.");
	printf("\n The inner loop prints even numbers between 1 to 10 \n\tfor every odd number printed by outer loop.");
	for(i = 1; i <= 10; i++) {
		if(i % 2 != 0) {
			printf("\n i = %d", i);
			printf("\n ------------\n");
			for(j = 1; j <= 10; j++) {
				if(j % 2 == 0)
					printf("\t j = %d", j);
				else
					continue;
			}
			printf("\n");
		}
		else
			continue;
	}

	printf("\n\n");
	return 0;
}