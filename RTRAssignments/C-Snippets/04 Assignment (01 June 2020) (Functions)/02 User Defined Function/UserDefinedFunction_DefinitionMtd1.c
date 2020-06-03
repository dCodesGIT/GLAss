// To demonstrate user defined function - method 1
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main(int argc, char *argv[], char *envp[]) {
	// Function declaration
	void AddIt();

	// Code
	AddIt();
	printf("\n\n");
	return 0;
}

void AddIt() {
	// Local Variable declaration
	int a, b, sum;

	// Code
	printf("\n Enter an integer, A : ");
	scanf("%d", &a);
	printf("\n Enter another integer, B : ");
	scanf("%d", &b);

	sum = a + b;
	printf("\n Sum of %d and %d is %d", a, b, sum);
}