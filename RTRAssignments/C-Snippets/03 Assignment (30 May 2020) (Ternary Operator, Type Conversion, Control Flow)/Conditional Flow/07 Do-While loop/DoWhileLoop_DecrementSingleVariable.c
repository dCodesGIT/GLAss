// To demonstrate DO WHILE loop - single variable decrement
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i;

	// Code
	printf("\n Printing value from 10 to 1...");
	i = 10;
	do {
		printf("\n %d", i);
		i--;
	} while(i > 0);

	printf("\n\n");
	return 0;
}