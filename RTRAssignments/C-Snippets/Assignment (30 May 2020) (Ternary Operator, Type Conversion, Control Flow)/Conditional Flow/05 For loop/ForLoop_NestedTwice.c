// To demonstrate nested FOR loop - two nested loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j, k;

	// Code
	for(i = 1; i <= 10; i++) {
		printf("\n i = %d", i);
		printf("\n ------------");
		for(j = 1; j <= 5; j++) {
			printf("\n \t j = %d", j);
			printf("\n \t -------------\n");
			for(k = 1; k <= 3; k++)
				printf("\t\t k = %d", k);
			printf("\n");
		}
		printf("\n");
	}

	printf("\n\n");
	return 0;
}