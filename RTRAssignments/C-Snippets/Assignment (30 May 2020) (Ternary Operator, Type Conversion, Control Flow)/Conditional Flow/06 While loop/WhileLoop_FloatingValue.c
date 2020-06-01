// To demonstrate WHILE loop - using floating values
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	float x=10.26, n;

	// Code
	printf("\n Printing numbers %f to %f...", x, (x * 10.0f));
	n = x;
	while(n <= (x * 10.0)) {
		printf("\n %f", n);
		n = n + x;
	}

	printf("\n\n");
	return 0;
}