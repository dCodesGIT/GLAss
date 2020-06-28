// To demonstrate pointers - 2D array Column memory allocation using pointers twice
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>

#define ROWS 5
#define COLS_ONE 3
#define COLS_TWO 8

int main() {
	// Variable declaration
	int i, j;
	int *iArray[ROWS];

	// Code
	printf("\n For 2D Array with %d columns", COLS_ONE);
	for(i = 0; i < ROWS; i++) {
		iArray[i] = (int *)malloc(sizeof(int) * COLS_ONE);
		if(iArray[i] == NULL) {
			printf("\n Memory allocation failure...");
			exit(0);
		}
		printf("\n Memory allocation succeded...");
	}

	for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS_ONE; j++)
			iArray[i][j] = (i + 1) * (j + 1);

	printf("\n Displaying 2D array using pointers...");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS_ONE; j++)
			printf("\n iArray[%d][%d] = %d", i, j, iArray[i][j]);
		printf("\n");
	}

	for(i = (ROWS - 1); i >= 0; i--) {
		free(iArray[i]);
		iArray[i] = NULL;
	}
	printf("\n Memory delocation succeded");

	printf("\n\n For 2D Array with %d columns", COLS_TWO);
	for(i = 0; i < ROWS; i++) {
		iArray[i] = (int *)malloc(sizeof(int) * COLS_TWO);
		if(iArray[i] == NULL) {
			printf("\n Memory allocation failure...");
			exit(0);
		}
		printf("\n Memory allocation succeded...");
	}

	for(i = 0; i < ROWS; i++)
		for(j = 0; j < COLS_TWO; j++)
			iArray[i][j] = (i + 1) * (j + 1);

	printf("\n Displaying 2D array using pointers...");
	for(i = 0; i < ROWS; i++) {
		for(j = 0; j < COLS_TWO; j++)
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