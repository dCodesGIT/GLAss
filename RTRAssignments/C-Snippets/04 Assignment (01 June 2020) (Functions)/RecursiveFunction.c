// To demonstrate recusive function
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	unsigned int num;

	// Function declaration
	void recursive(unsigned int);

	// Code
	printf("\n Enter any number : ");
	scanf("%d", &num);
	printf("\n Output of recursive function is : ");
	recursive(num);

	printf("\n\n");
	return 0;
}

void recursive(unsigned int x) {
	printf("\n %d", x);
	if(x > 0)
		recursive(x - 1);
}