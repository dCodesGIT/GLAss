// To demonstrate 1 dimensional array - Seperate even and odd elements of array
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define ARRAY_SIZE 10
int main() {
	// Variable declaration
	int iArray[ARRAY_SIZE];
	int i;

	// Code
	printf("\n Enter 10 integers for an array : \n");
	for(i = 0; i < ARRAY_SIZE; i++)
		scanf("%d", &iArray[i]);

	printf("\n\n Even numbers in above array are : \n");
	for(i = 0; i < ARRAY_SIZE; i++) {
		if((iArray[i] % 2) == 0)
			printf("%d \t", iArray[i]);
	}

	printf("\n Odd numbers in above array are : \n");
	for(i = 0; i < ARRAY_SIZE; i++) {
		if((iArray[i] % 2) == 1)
			printf("%d \t", iArray[i]);
	}

	printf("\n\n");
	return 0;
}