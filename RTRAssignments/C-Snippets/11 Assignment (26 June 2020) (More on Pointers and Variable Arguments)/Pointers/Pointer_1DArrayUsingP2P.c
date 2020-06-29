// To demonstrate pointers - 1D array using pointer to pointer
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>
int main() {
	// Function declaration
	void MyAlloc(int **, unsigned int);

	// Variable declaration
	int *ptr_iArray = NULL;
	unsigned int n;
	int i;

	// Code
	printf("\n How many elements you want in integer array : ");
	scanf("%u", &n);
	
	MyAlloc(&ptr_iArray, n);
	
	printf("\n Now enter %u elements of integer array ", n);
	for(i = 0; i < n; i++) {
		printf(" %d. ", i + 1);
		scanf("%d", &ptr_iArray[i]);
	}
	
	printf("\n Elements entered by you are");
	for(i = 0; i < n; i++)
		printf("\n %d. %d", i + 1, ptr_iArray[i]);

	if(ptr_iArray) {
		free(ptr_iArray);
		ptr_iArray = NULL;
	}

	printf("\n\n");
	return 0;
}

// Function definition
void MyAlloc(int **pptr, unsigned int size) {
	// Code
	*pptr = (int *)malloc(sizeof(int) * size);
	if(*pptr == NULL) {
		printf("\n Memory allocation failed !!!");
		exit(0);
	}
}