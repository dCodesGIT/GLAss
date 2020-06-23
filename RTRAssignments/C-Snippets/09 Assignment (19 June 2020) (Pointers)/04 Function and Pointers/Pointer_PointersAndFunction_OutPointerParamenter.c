// To demonstrate pointers - pointers and functions - pointers as out parameters
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Function declaration
	void MathematicalOperations(int, int, int *, int *, int *, int *, int *);

	// Variable declaration
	int a, b;
	int sum, diff, pro, quo, rem;

	// Code
	printf("\n Enter a number, A : ");
	scanf("%d", &a);
	printf("\n Enter another number, B : ");
	scanf("%d", &b);
	MathematicalOperations(a, b, &sum, &diff, &pro, &quo, &rem);
	printf("\n Result of %d and %d is", a, b);
	printf("\n Sum = %d", sum);
	printf("\n Difference = %d", diff);
	printf("\n Product = %d", pro);
	printf("\n Quotient = %d", quo);
	printf("\n Remainder = %d", rem);

	printf("\n\n");
	return 0;
}

// Function definition
void MathematicalOperations(int x, int y, int *add, int *sub, int *mul, int *quo, int *rem) {
	// Code
	*add = x + y;
	*sub = x - y;
	*mul = x * y;
	*quo = x / y;
	*rem = x % y;
}