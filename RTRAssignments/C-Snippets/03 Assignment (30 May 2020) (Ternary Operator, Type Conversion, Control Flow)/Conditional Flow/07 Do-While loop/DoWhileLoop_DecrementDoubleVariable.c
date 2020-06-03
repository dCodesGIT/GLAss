// To demonstrate DO WHILE loop - double variable decrement
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	i = 10;
	j = 100;
	do {
		printf("\n %d \t %d", i, j);
		i--;
		j = j - 10;
	} while(i > 0, j >= 10);

	printf("\n\n");
	return 0;
}