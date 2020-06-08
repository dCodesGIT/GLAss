// To demonstrate 1 dimensional array - Prime numbers in array
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define ARRAY_SIZE 10
int main() {
	// Variable declaration
	int iArray[ARRAY_SIZE];
	int i, j, count;

	// Code
	printf("\n Enter 10 elements in array : ");
	for(i = 0; i < ARRAY_SIZE; i++) {
		scanf("%d", &iArray[i]);
		if(iArray[i] < 0)
			iArray[i] *= (-1);
	}

	printf("\n Prime numbers in the array are : ");
	for(i = 0; i < ARRAY_SIZE; i++) {
		count = 0;
		for(j = 2; j < iArray[i]; j++) {
			if((iArray[i] % j) == 0) {
				count++;
			}
		}
		if(count == 0)
			printf("%d\t", iArray[i]);
	}

	printf("\n\n");
	return 0;
}