// Demonstration of basic arithmetic operators

#include <stdio.h>
int main() {
	// Variable declarations
	int a, b, result;
	
	// Code
	printf("\n Enter number A : ");
	scanf("%d", &a);
	printf("\n Enter number B : ");
	scanf("%d", &b);

	// Following are the five basi arithmetic operations
	result = a + b;
	printf("\n Addition of A = %d and B = %d is %d", a, b, result);

	result = a - b;
	printf("\n Subtration of A = %d and B = %d is %d", a, b, result);

	result = a * b;
	printf("\n Multiplication of A = %d and B = %d is %d", a, b, result);

	result = a / b;
	printf("\n Division of A = %d and B = %d gives quotiont %d", a, b, result);

	result = a % b;
	printf("\n Division of A = %d and B = %d gives remainder %d", a, b, result);

	printf("\n\n");
	return 0;
}