// Demonstration of Compound arithmetic operators

#include <stdio.h>
int main() {
	// Variable declaration
	int a, b, x;

	// Code
	printf("\n Enter number A : ");
	scanf("%d", &a);
	printf("\n Enter number B : ");
	scanf("%d", &b);

	// Use of Compound Arithmetic operators 
	x = a;
	a += b;
	printf("\n Addition of A = %d and B = %d is %d", x, b, a);

	x = a;
	a -= b;
	printf("\n Subtraction of A = %d and B = %d is %d", x, b, a);

	x = a;
	a *= b;
	printf("\n Multiplication of A = %d and B = %d is %d", x, b, a);

	x = a;
	a /= b;
	printf("\n Division of A = %d and B = %d gives quotient %d", x, b, a);

	x = a;
	a %= b;
	printf("\n Division of A = %d and B = %d gives remainder %d", x, b, a);

	printf("\n\n");
	return 0;
}