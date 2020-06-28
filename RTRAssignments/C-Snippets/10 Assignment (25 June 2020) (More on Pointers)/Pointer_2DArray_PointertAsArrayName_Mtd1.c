// To demonstrate pointers - 2D array as pointers (As name of array) - Method 1
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>

#define ROWS 5
#define COLS 3

int main() {
	// Variable declaration
	int i, j;
	int iArray[ROWS][COLS];

	// Code
	for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS; j++)
			*(iArray[i] + j) = (i + 1) * (j + 1);

	printf("\n 2D array elements along with addresses is");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++)
			printf("\n *(iArray[%d] + %d) = %d \t\t is at address = %p", i, j, *(iArray[i] + j), (iArray[i] + j));
		printf("\n");
	}

	printf("\n\n");
	return 0;
}