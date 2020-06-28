// To demonstrate pointers - 2D array as pointers (As pointers only) - Method 1
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>

#define ROWS 5
#define COLS 3

int main() {
	// Variable declaration
	int i, j;
	int iArray[ROWS][COLS];
	int *iArrayRow_ptr = NULL;

	// Code
	for(i = 0; i < ROWS; i++) {
		iArrayRow_ptr = iArray[i];
		for(j = 0; j < COLS; j++)
			*(iArrayRow_ptr + j) = (i + 1) * (j + 1);
	}

	printf("\n 2D array elements along with addresses is");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++)
			printf("\n *(iArrayRow_ptr + %d) = %d \t\t is at address = %p", j, *(iArrayRow_ptr + j), (iArrayRow_ptr + j));
		printf("\n");
	}

	printf("\n\n");
	return 0;
}