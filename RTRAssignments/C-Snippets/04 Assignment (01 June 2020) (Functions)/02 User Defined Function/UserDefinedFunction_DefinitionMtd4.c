// To demonstrate user defined function - definition method 4
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main(int argc, char **argv, char **envp) {
	// Function Declaration
	int addIt(int, int);

	// Variable declaration
	int a, b, sum;

	// Code
	printf("\n Enter an integer, A : ");
	scanf("%d", &a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &b);

	sum=addIt(a, b);
	printf("\n Sum = %d", sum);
	
	printf("\n\n");
	return 0;
}

// Function definition
int addIt(int p, int q) {
	return(p + q);
}