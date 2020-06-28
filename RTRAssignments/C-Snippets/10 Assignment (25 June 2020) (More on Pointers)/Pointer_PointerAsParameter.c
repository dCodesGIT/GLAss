// To demonstrate pointers - Integer array as pointer parameters
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>
int main() {
	// Function declaration
	void MultiplyArrayElements(int *, int, int);

	// Variable declaration
	int i, num,	n_elements;
	int *iArray = NULL;

	// Code
	printf("\n How many elements you want in integer array : ");
	scanf("%d", &n_elements);
	iArray = (int *)malloc(sizeof(int) * n_elements);
	if(iArray == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}
	printf("\n Now enter %d elements in integer array \n", n_elements);
	for(i = 0; i < n_elements; i++) {
		printf(" %d. ", i + 1);
		scanf("%d", (iArray + i));
	}

	printf("\n Array before any modification");
	for(i = 0; i < n_elements; i++)
		printf("\n %d. %d", i + 1, *(iArray + i));

	printf("\n\n Enter a number to multiply with each element of array : ");
	scanf("%d", &num);
	MultiplyArrayElements(iArray, n_elements, num);

	printf("\n Modified array is ");
	for(i = 0; i < n_elements; i++)
		printf("\n %d. %d", i + 1, *(iArray + i));

	if(iArray) {
		free(iArray);
		iArray = NULL;
		printf("\n\n Memory deallocated successfully");
	}

	printf("\n\n");
	return 0;
}

// Function declaration
void MultiplyArrayElements(int *arr, int size, int modifier) {
	// Variable declaration
	int i;

	// Code
	for(i = 0; i < size; i++)
		arr[i] *= modifier;
}