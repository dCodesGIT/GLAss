// To demonstrate DO WHILE loop - using floating values
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	float n, x = 2.6f;

	// Code
	printf("\n Printing numbers from %f to %f.", x, (x * 10.0f));
	n = x;
	do {
		printf("\n %f", n);
		n = n + x;
	} while(n <= (x * 10.0));

	printf("\n\n");
	return 0;
}