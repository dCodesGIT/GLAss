// To demonstrate user defined function - definition method 2
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main(int argc, char **argv, char **envp) {
	// Function Declaration
	int Adder(void);

	// Variable declaration
	int ans;

	// Code
	ans = Adder();
	printf("\n Sum = %d", ans);

	printf("\n\n");
	return 0;
}

// Function definition
int Adder(void) {
	// Local variable declaration
	int a, b;
	
	// Code
	printf("\n Enter an integer, A : ");
	scanf("%d", &a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &b);

	return (a + b);
}