// To demonstrate nested DO WHILE loop - one nested loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	i = 1;
	do {
		printf("\n i = %d", i);
		printf("\n ------------\n");
		j = 1;
		do {
			printf("\t\t j = %d", j);
			j++;
		} while(j <= 5);
		i++;
		printf("\n");
	} while(i <= 10);

	printf("\n\n");
	return 0;
}