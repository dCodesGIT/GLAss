// To demonstrate pointer - Addresses of integer arrays using pointers
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int iArray[10], i;
	int *iArray_ptr;

	// Code
	for(i = 0; i < 10; i++)
		iArray[i] = (i + 1) * 10;
	iArray_ptr = iArray;
	printf("\n Elements in integer array and thier addresses are");
	for(i = 0; i < 10; i++)
		printf("\n iArray[%d] = %d \t Address = %p", i, *(iArray_ptr + i), (iArray_ptr + i));

	printf("\n\n");
	return 0;
}