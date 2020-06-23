// To demonstrate pointer - Addresses of float arrays using pointers
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int i;
	float fArray[10];
	float *fArray_ptr = NULL;

	// Code
	for(i = 0; i < 10; i++)
		fArray[i] = (float)(i + 1) * 2.6f;
	fArray_ptr = fArray;
	printf("\n Elements in float array and thier addresses are");
	for(i = 0; i < 10; i++)
		printf("\n fArray[%d] = %f \t Address = %p", i, *(fArray_ptr + i), (fArray_ptr + i));

	printf("\n\n");
	return 0;
}