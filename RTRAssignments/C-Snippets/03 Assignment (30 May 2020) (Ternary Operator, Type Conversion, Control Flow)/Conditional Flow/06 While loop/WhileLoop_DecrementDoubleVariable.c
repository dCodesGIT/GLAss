// To demonstrate WHILE loop - double variable decrementing
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declration
	int i, j;

	// Code
	printf("\n Printing digits 10 to 1 and 100 to 10..");
	i = 10;
	j = 100;
	while(i >= 1, j >= 10) {
		printf("\n %d \t %d", i, j);
		i--;
		j = j - 10;
	}

	printf("\n\n");
	return 0;
}