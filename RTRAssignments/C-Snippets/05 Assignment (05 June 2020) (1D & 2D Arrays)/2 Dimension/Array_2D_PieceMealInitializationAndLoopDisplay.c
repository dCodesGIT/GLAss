// To demonstrate 2 dimensional array - PieceMeal initialization and loop display
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int iArray[3][5];
	int int_size, iArray_size;
	int iArray_rows, iArray_cols, iArray_n;
	int i, j;

	// Code
	int_size = sizeof(int);
	iArray_size = sizeof(iArray);
	printf("\n Size of 2D integer array is %d", iArray_size);
	iArray_rows = iArray_size / sizeof(iArray[0]);
	printf("\n Number of rows in 2D array are %d", iArray_rows);
	iArray_cols = sizeof(iArray[0]) / int_size;
	printf("\n Number of columns in 2D array are %d", iArray_cols);
	iArray_n = iArray_rows * iArray_cols;
	printf("\n Number of elements in 2D array are %d", iArray_n);

	iArray[0][0] = 10;
	iArray[0][1] = 20;
	iArray[0][2] = 30;
	iArray[0][3] = 40;
	iArray[0][4] = 50;

	iArray[1][0] = 60;
	iArray[1][1] = 70;
	iArray[1][2] = 80;
	iArray[1][3] = 90;
	iArray[1][4] = 100;

	iArray[2][0] = 110;
	iArray[2][1] = 120;
	iArray[2][2] = 130;
	iArray[2][3] = 140;
	iArray[2][4] = 150;

	for(i = 0; i < iArray_rows; i++) {
		printf("\n\n *** ROW %d ***",i+1);
		for(j = 0; j < iArray_cols; j++)
			printf("\n iArray[%d][%d] : %d", i, j, iArray[i][j]);
	}

	printf("\n\n");
	return 0;
}