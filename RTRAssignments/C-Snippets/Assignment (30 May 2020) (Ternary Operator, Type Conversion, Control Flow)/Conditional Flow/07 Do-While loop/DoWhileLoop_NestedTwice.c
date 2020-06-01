// To demonstrate nested DO WHILE loop - two nested loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j, k;

	// Code
	i = 1;
	do {
		printf("\n i = %d", i);
		printf("\n ------------");
		j = 1;
		do {
			printf("\n\t j = %d", j);
			printf("\n\t -------------\n");
			k = 1;
			do {
				printf("\t\t k = %d", k);
				k++;
			} while(k <= 3);
			j++;
			printf("\n");
		} while(j <= 5);
		i++;
		printf("\n");
	} while(i <= 10);

	printf("\n\n");
	return 0;
}