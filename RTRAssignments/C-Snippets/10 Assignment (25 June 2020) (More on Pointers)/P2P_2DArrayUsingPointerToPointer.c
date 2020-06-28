// To demonstrate pointer of pointer - 2D array using pointer of pointer
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>
int main() {
	// Variable declaration
	int rows, cols, i, j;
	int **iArray = NULL;

	// Code
	printf("\n Enter number of rows : ");
	scanf("%d", &rows);
	printf("\n Enter number of columns : ");
	scanf("%d", &cols);
	iArray = (int **)malloc(sizeof(int *) * rows);
	if(iArray==NULL) {
		printf("\n Memory allocation failed");
		exit(0);
	}
	for(i = 0; i < rows; i++) {
		iArray[i] = (int *)malloc(sizeof(int) * cols);
		if(iArray[i] == NULL) {
			printf("\n Memory allocation failed");
			exit(0);
		}
	}
	printf("\n Memory allocated successfully");

	for(i = 0; i < rows; i++)
		for(j = 0; j < cols; j++)
			iArray[i][j] = (i + 1) * (j + 1);

	printf("\n Elements of 2D array are");
	for(i = 0; i < rows; i++) {
		printf("\n Base address of row %d = %p", i, &iArray[i]);
		for(j = 0; j < cols; j++)
			printf("\n iArray[%d][%d] = %d \t Address = %p", i, j, iArray[i][j], &iArray[i][j]);
	}

	for(i = rows - 1; i >= 0; i--) {
		if(iArray[i]) {
			free(iArray[i]);
			iArray[i] = NULL;
		}
	}
	if(iArray) {
		free(iArray);
		iArray = NULL;
	}
	printf("\n Memory deallocated sucessfully !!!!");

	printf("\n\n");
	return 0;
}