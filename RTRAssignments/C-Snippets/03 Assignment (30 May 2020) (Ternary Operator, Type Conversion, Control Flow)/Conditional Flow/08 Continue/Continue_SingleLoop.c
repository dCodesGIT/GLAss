// To demonstrate CONTINUE statement - using one simple loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable decclaration
	int i;

	// Code
	printf("\n Printing even numbers from 0 to 100..");
	printf("\n\n");
	for(i = 0; i <= 100; i++) {
		if(i % 2 == 0)
			printf(" %d \t", i);
		else
			continue;
	}

	printf("\n\n");
	return 0;
}