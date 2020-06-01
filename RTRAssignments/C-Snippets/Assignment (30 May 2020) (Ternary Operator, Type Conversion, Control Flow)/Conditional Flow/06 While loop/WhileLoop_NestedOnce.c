// To demonstrate WHILE loop - one nested loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;

	// Code
	i = 1;
	while(i <= 10) {
		printf("\n i = %d ", i);
		printf("\n -------------\n");
		j = 1;
		while(j <= 5) {
			printf("\t j = %d", j);
			j++;
		}
		i++;
		printf("\n");
	}

	printf("\n\n");
	return 0;
}