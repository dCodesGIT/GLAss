// Using of variables in C
#include <stdio.h>

int main() {
	// Code
	// Variable declaration
	int i = 5;
	float f = 3.142f;
	double d = 8.01015934;
	char c = 'D';

	// Printing of variable with values
	printf("\n i = %d", i);
	printf("\n f = %f", f);
	printf("\n d = %lf", d);
	printf("\n c = %c", c);
	printf("\n\n");

	// Changing of values of variables
	i = 43;
	f = 6.022f;
	d = 1024.204869;
	c = 'X';

	// Printing of changed values
	printf("\n i = %d", i);
	printf("\n f = %f", f);
	printf("\n d = %lf", d);
	printf("\n c = %c", c);
	printf("\n\n");

	return 0;
}