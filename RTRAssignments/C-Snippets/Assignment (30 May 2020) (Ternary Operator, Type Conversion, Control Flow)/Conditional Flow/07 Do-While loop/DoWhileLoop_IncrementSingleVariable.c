// To demonstrate DO WHILE loop - single variable increment
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i;

	// Code
	printf("\n Printing digits from 1 to 10...");
	i = 1;
	do {
		printf("\n %d", i);
		i++;
	} while(i <= 10);

	printf("\n\n");
	return 0;
}