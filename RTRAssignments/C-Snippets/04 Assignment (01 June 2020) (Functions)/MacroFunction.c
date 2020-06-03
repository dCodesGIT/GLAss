// To demonstrate MACRO FUNCTION
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define MAX(a,b) ((a>b)?a:b)		// Macro function
int main() {
	// Variable declaration
	int i1, i2, iMax;
	float f1, f2, fMax;

	// Code
	printf("\n\n Enter an integer : ");
	scanf("%d", &i1);
	printf("\n Enter another integer : ");
	scanf("%d", &i2);
	iMax = MAX(i1, i2);
	printf("\n MAX() macro function result is %d", iMax);

	printf("\n\n Enter a floating number : ");
	scanf("%f", &f1);
	printf("\n Enter another floating number : ");
	scanf("%f", &f2);
	fMax = MAX(f1, f2);
	printf("\n MAX() macro function result is %f", fMax);

	printf("\n\n");
	return 0;
}