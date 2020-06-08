// To demonstrate 1 dimensional array - Sum of array elements
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define ARRAY_SIZE 10
int main() {
	// Variable declaration
	int iArray[ARRAY_SIZE];
	int i, sum;

	// Code
	sum = 0;
	printf("\n Enter 10 integer elements for array : \n");
	for(i = 0; i < ARRAY_SIZE; i++) {
		scanf("%d", &iArray[i]);
		sum = sum + iArray[i];
	}
	printf("\n Sum of all elements of array = %d", sum);

	printf("\n\n");
	return 0;
}