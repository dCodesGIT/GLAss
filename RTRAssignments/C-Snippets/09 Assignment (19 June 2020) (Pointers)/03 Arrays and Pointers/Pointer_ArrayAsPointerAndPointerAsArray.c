// To demonstrate pointer - Arrays as pointers and pointers as arrays
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int iArray[] = { 10,20,30,40,50,60,70,80,90,100 };
	int *iArray_ptr = NULL;
	int i;

	// Code
	printf("\n USING ARRAY NAME AS A POINTER");
	printf("\n Value of xth element = *(iArray + x) and Address of xth element : (iArray + x)");
	printf("\n Integer array elements and their addresses");
	for(i = 0; i < 10; i++)
		printf("\n *(iArray + %d) = %d \t\t (iArray + %d) : %p", i, *(iArray + i), i, (iArray + i));

	iArray_ptr = iArray;
	printf("\n\n USING POINTER AS ARRAY NAME");
	printf("\n Value of xth element = iArray_ptr[x] and Address of xth element : &iArray_ptr[x]");
	printf("\n Integer array elements and their addresses");
	for(i = 0; i < 10; i++)
		printf("\n iArray_ptr[%d] = %d \t\t &iArray_ptr[%d] : %p", i, iArray_ptr[i], i, &iArray_ptr[i]);

	printf("\n\n");
	return 0;
}