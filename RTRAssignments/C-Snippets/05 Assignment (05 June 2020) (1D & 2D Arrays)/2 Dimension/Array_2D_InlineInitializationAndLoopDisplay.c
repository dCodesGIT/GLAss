// To demonstrate 2 dimensional array - Inline initialization and loop display
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i2DArray[5][3] = { {1,2,3},{2,4,6},{3,6,9},{4,8,12},{5,10,15} };
	int int_size, i2DArray_size, i2DArray_n;
	int i2DArray_cols, i2DArray_rows;
	int i, j;

	// Code
	int_size = sizeof(int);
	i2DArray_size = sizeof(i2DArray);
	printf("\n\n Size of 2 Dimensional integer array is %d", i2DArray_size);
	i2DArray_rows = i2DArray_size / sizeof(i2DArray[0]);
	printf("\n\n Number of rows in 2D array are %d", i2DArray_rows);
	i2DArray_cols = sizeof(i2DArray[0]) / int_size;
	printf("\n\n Number of cloumns in 2D array are %d", i2DArray_cols);
	i2DArray_n = i2DArray_rows * i2DArray_cols;
	printf("\n\n Number of elements in 2D array are %d", i2DArray_n);

	printf("\n\n Elements of 2D array are : ");
	for(i = 0; i < i2DArray_rows; i++) {
		printf("\n *** ROW %d ***", i + 1);
		for(j = 0; j < i2DArray_cols; j++)
			printf("\n i2DArray[%d][%d] : %d", i, j, i2DArray[i][j]);
		printf("\n");
	}

	printf("\n\n");
	return 0;
}