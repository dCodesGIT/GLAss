// To demonstrate WHILE loop - two nested loops
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j, k;

	// Code
	i = 1;
	while(i <= 10) {
		printf("\n i = %d ", i);
		printf("\n -------------\n");
		j = 1;
		while(j <= 5) {
			printf("\n \t j = %d", j);
			printf("\n \t -------------\n");
			k = 1;
			while(k <= 3) {
				printf("\t\t k = %d", k);
				k++;
			}
			printf("\n");
			j++;
		}
		i++;
		printf("\n");
	}

	printf("\n\n");
	return 0;
}