// To demonstrate 1 dimensional array - User input array	
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define INT_ARRAY_N 5
#define FLOAT_ARRAY_N 3
#define CHAR_ARRAY_N 15

int main() {
	// Variable declaration
	int iArray[INT_ARRAY_N];
	float fArray[FLOAT_ARRAY_N];
	char cArray[CHAR_ARRAY_N];
	int i;

	// Code
	printf("\n Enter the elements of integer array 'iArray[]' : ");
	for(i = 0; i < INT_ARRAY_N; i++)
		scanf("%d", &iArray[i]);
	printf("\n Enter the elements of float array 'fArray[]' : ");
	for(i = 0; i < FLOAT_ARRAY_N; i++)
		scanf("%f", &fArray[i]);
	printf("\n Enter the elements of character array 'cArray[]' : ");
	for(i = 0; i < CHAR_ARRAY_N; i++)
		scanf(" %c", &cArray[i]);

	printf("\n Integer array entered by you is : \n");
	for(i = 0; i < INT_ARRAY_N; i++)
		printf("\t %d", iArray[i]);
	printf("\n Float array entered by you is : \n");
	for(i = 0; i < FLOAT_ARRAY_N; i++)
		printf("\t %f", fArray[i]);
	printf("\n Character array entered by you is : \n");
	for(i = 0; i < CHAR_ARRAY_N; i++)
		printf(" %c", cArray[i]);

	printf("\n\n");
	return 0;
}