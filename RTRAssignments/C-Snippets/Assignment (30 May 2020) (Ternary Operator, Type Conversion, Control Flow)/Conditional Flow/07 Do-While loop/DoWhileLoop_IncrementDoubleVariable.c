// To demonstrate DO WHILE loop - double variable increment
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	printf("\n Printing digits from 1 to 10 and 10 to 100...");
	i = 1;
	j = 10;
	do {
		printf("\n %d \t %d", i, j);
		i++;
		j+=10;
	} while(i <= 10, j <= 100);

	printf("\n\n");
	return 0;
}