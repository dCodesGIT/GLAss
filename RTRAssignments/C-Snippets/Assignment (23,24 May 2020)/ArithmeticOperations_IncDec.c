// Demonstration of increment and decrement operators

#include <stdio.h>
int main() {
	// Variable declaration
	int a = 5;
	int b = 10;

	// Code
	printf("\n\n A = %d", a);
	printf("\n A = %d", a++);
	printf("\n A = %d", a);
	printf("\n A = %d", ++a);

	printf("\n\n B = %d", b);
	printf("\n B = %d", b--);
	printf("\n B = %d", b);
	printf("\n B = %d", --b);

	printf("\n\n");
	return 0;
}