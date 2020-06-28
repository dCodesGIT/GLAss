// To demonstrate pointers - Memory allocation and deallocation using malloc() and free()
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>
int main() {
	// Variable declaration
	int *iArray_ptr = NULL;
	int i;
	unsigned int iArray_length = 0;
	
	// Code
	printf("\n Enter the number of integer elements you want in array : ");
	scanf("%d", &iArray_length);

	iArray_ptr = (int *)malloc(sizeof(int) * iArray_length);
	if(iArray_ptr == NULL) {
		printf("\n Memory allocation failed !!!");
		printf("\n Exiting now");
		exit(0);
	}
	printf("\n Memory allocation succeded");
	printf("\n Addresses from %p to %p have been allocated.", iArray_ptr, iArray_ptr + (iArray_length - 1));
	printf("\n Enter %d elements for integer array\n", iArray_length);
	for(i = 0; i < iArray_length; i++) {
		printf(" %d. ", i + 1);
		scanf("%d", iArray_ptr + i);
	}
	printf("\n\n The integer array you entered is (Displaying as Array)");
	for(i = 0; i < iArray_length; i++)
		printf("\n iArray[%d] = %d \t Address = %p", i, iArray_ptr[i], &iArray_ptr[i]);
	printf("\n\n The integer array you entered is (Displaying as pointer)");
	for(i = 0; i < iArray_length; i++)
		printf("\n *(iArray_ptr + %d) = %d \t Address = %p", i, *(iArray_ptr + i), (iArray_ptr + i));
	if(iArray_ptr) {
		free(iArray_ptr);
		iArray_ptr = NULL;
		printf("\n Memory deallocated successfully");
	}
	printf("\n\n");
	return 0;
}