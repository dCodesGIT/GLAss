// To demonstrate pointer - Addresses of integer arrays without using pointers
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int iArray[10], i;

	// Code
	for(i = 0; i < 10; i++)
		iArray[i] = (i + 1) * 10;
	printf("\n Elements in integer array and thier addresses are");
	for(i = 0; i < 10; i++)
		printf("\n iArray[%d] = %d \t Address = %p", i, iArray[i], &iArray[i]);

	printf("\n\n");
	return 0;
}