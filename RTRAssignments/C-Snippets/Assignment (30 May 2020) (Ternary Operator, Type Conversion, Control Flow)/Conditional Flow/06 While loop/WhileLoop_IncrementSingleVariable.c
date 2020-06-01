// To demonstrate WHILE loop - single variable incrementing
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declration
	int i;

	// Code
	printf("\n Printing digits 1 to 10..");
	i = 1;
	while(i <= 10) {
		printf("\n %d", i);
		i++;
	}

	printf("\n\n");
	return 0;
}