// To demonstrate pointers - 2D array as pointers (As pointer only) - Method 2
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define ROWS 5
#define COLS 3

int main() {
	// Variable declaration
	int i, j;
	int **iArray_ptr = NULL;

	// Code
	iArray_ptr = (int **)malloc(sizeof(int *) * ROWS);
	if(iArray_ptr == NULL) {
		printf("\n Memory allocation for 1D array of %d rows failed", ROWS);
		printf("\n Exitting now !!!");
		exit(0);
	}
	printf("\n Memory allocation for 1D array of %d rows succeded", ROWS);
	for(i = 0; i < ROWS; i++) {
		iArray_ptr[i] = (int *)malloc(sizeof(int) * COLS);
		if(iArray_ptr == NULL) {
			printf("\n Memory allocation for 2D array of %d row failed", ROWS);
			printf("\n Exitting now !!!");
			exit(0);
		}
		printf("\n Memory allocation for 2D array %d row succeded", ROWS);
	}
	for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS; j++)
			*(*(iArray_ptr + i) + j) = (i + 1) * (j + 1);

	printf("\n 2D array elements along with addresses is");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++)
			printf("\n iArray_ptr[%d][%d] = %d \t\t is at address = %p", i, j, iArray_ptr[i][j], &iArray_ptr[i][j]);
		printf("\n");
	}

	for(i = ROWS - 1; i >= 0; i--) {
		if(*(iArray_ptr + i) != NULL) {
			free(iArray_ptr[i]);
			*(iArray_ptr + 1) = NULL;
			printf("\n  Memory allocated for %d row freed successfully", i);
		}
	}
	if(iArray_ptr != NULL) {
		free(iArray_ptr);
		iArray_ptr = NULL;
		printf("\n Memory allocated for 1D array freed successfully");
	}

	printf("\n\n");
	return 0;
}