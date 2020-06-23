// To demonstrate pointers - pointers and function - swapping numbers without pointer parameters
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Function declaration
	void swap(int, int);

	// Variable declaration
	int a, b;

	// Code
	printf("\n Enter a number, A : ");
	scanf("%d", &a);
	printf("\n Enter another number, B : ");
	scanf("%d", &b);

	printf("\n *** Before Swapping ****");
	printf("\n Value of 'A' = %d", a);
	printf("\n Value of 'B' = %d", b);
	swap(a, b);
	printf("\n\n *** After Swapping ****");
	printf("\n Value of 'A' = %d", a);
	printf("\n Value of 'B' = %d", b);

	printf("\n\n");
	return 0;
}

// Function definition
void swap(int x, int y) {
	// Variable declaration
	int temp;

	// Code
	printf("\n\n *** Before Swapping ****");
	printf("\n Value of 'X' = %d", x);
	printf("\n Value of 'Y' = %d", y);

	temp = x;
	x = y;
	y = temp;

	printf("\n\n *** After Swapping ****");
	printf("\n Value of 'X' = %d", x);
	printf("\n Value of 'Y' = %d", y);
}