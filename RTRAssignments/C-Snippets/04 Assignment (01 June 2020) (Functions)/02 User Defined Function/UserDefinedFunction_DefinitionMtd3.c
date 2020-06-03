// To demonstrate user defined function - definition method 3
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main(int argc, char **argv, char **envp) {
	// Function Declaration
	void addIt(int, int);

	// Variable declaration
	int a, b;

	// Code
	printf("\n Enter an integer, A : ");
	scanf("%d", &a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &b);

	addIt(a, b);

	printf("\n\n");
	return 0;
}

// Function definition
void addIt(int p, int q) {
	// Local variable declaration
	int result;

	// Code
	result = p + q;
	printf("\n Sum = %d", result);
}