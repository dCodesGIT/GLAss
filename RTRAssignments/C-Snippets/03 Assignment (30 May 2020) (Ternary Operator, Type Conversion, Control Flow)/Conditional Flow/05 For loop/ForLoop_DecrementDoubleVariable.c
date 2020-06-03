// To demonstrate FOR loop - two variable decrementing
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	printf("\n Printing digits 10 to 1 and 100 to 10...");
	for(i = 10, j = 100; i > 0, j >= 10; i--, j-=10)
		printf("\n %d \t %d", i, j);

	printf("\n\n");
	return 0;
}