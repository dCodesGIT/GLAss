// To demonstrate FOR loop - two variable incrementing
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	printf("\n Printing digits 1 to 10 and 10 to 100...");
	for(i = 1, j = 10; i <= 10, j <= 100; i++, j+=10)
		printf("\n %d \t %d", i, j);

	printf("\n\n");
	return 0;
}