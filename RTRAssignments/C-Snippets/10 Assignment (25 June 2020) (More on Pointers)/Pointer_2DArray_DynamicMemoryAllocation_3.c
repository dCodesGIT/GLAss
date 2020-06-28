// To demonstrate pointers - 2D array variable column memory allocation using pointers
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define ROWS 5
#define COLS 5

int main() {
	// Variable decalaration
	int i, j;
	int *iArray[ROWS];

	// Code
	for(i = 0; i < ROWS; i++) {
		iArray[i] = (int *)malloc(sizeof(int) * (COLS - i));
		if(iArray[i] == NULL) {
			printf("\n Memory allocation failed !!!");
			exit(0);
		}
		printf("\n Memory allocated sucessfully");
	}

	for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS - 1; j++)
			iArray[i][j] = (i + 1) * (j + 1);

	printf("\n Elements of 2D array and its addresses are");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS - 1; j++)
			printf("\n iArray[%d][%d] = %d \t Address = %p", i, j, iArray[i][j], &iArray[i][j]);
		printf("\n");
	}

	for(i = ROWS - 1; i >= 0; i--) {
		if(iArray[i]) {
			free(iArray[i]);
			iArray[i] = NULL;
		}
	}
	printf("\n Memory delocated sucessfully !!!");

	printf("\n\n");
	return 0;
}