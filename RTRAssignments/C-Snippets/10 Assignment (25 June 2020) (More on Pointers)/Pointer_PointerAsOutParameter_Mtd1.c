// To demonstrate pointers - pointers and functions - pointers as out parameters method 1
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Function declaration
	void MathematicalOperation(int, int, int *, int *, int *, int *, int *);

	// Variable declaration
	int a, b;
	int sum, diff, pro, quo, rem;

	// Code
	printf("\n Enter a number : ");
	scanf("%d", &a);
	printf("\n Enter another number : ");
	scanf("%d", &b);

	MathematicalOperation(a, b, &sum, &diff, &pro, &quo, &rem);

	printf("\n *** Result ***");
	printf("\n Sum = %d", sum);
	printf("\n Difference = %d", diff);
	printf("\n Product = %d", pro);
	printf("\n Quotient = %d", quo);
	printf("\n Remainder = %d", rem);

	printf("\n\n");
	return 0;
}

// Function definition
void MathematicalOperation(int x, int y, int *add, int *sub, int *mult, int *quo, int *rem) {
	// Code
	*add = x + y;
	*sub = x - y;
	*mult = x * y;
	*quo = x / y;
	*rem = x % y;
}