// To demonstrate pointers - 2D array Column memory allocation using pointers once
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define ROWS 5
#define COLS 3

int main() {
	// Variable declaration
	int i, j;
	int *iArray[ROWS];

	// Code
	for(i = 0; i < ROWS; i++) {
		iArray[i] = (int *)malloc(sizeof(int) * COLS);
		if(iArray[i] == NULL) {
			printf("\n Memory allocation failure...");
			exit(0);
		}
		printf("\n Memory allocation succeded...");
	}

	for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS; j++)
			iArray[i][j] = (i + 1) * (j + 1);

	printf("\n Displaying 2D array using pointers...");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS; j++)
			printf("\n iArray[%d][%d] = %d", i, j, iArray[i][j]);
		printf("\n");
	}

	for(i = (ROWS - 1); i >= 0; i--) {
		free(iArray[i]);
		iArray[i] = NULL;
	}
	printf("\n Memory delocation succeded");
	
	printf("\n\n");
	return 0;
}