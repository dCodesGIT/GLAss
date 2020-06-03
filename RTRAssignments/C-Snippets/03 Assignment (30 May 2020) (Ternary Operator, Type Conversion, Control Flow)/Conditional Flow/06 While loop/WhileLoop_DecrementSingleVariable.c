// To demonstrate WHILE loop - single variable decrementing
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declration
	int i;

	// Code
	printf("\n Printing digits 10 to 1..");
	i = 10;
	while(i > 0) {
		printf("\n %d", i);
		i--;
	}

	printf("\n\n");
	return 0;
}