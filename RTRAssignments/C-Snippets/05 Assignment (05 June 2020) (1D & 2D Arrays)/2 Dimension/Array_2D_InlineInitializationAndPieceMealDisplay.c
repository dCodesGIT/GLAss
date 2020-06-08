// To demonstrate 2 dimensional array - Inline initialization and piece meal display
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i2DArray[5][3] = { {1,2,3},{2,4,6},{3,6,9},{4,8,12},{5,10,15} };
	int int_size, i2DArray_size, i2DArray_n;
	int i2DArray_cols, i2DArray_rows;

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
	printf("\n *** ROW 1 ***");
	printf("\n i2DArray[0][0] : %d", i2DArray[0][0]);
	printf("\n i2DArray[0][1] : %d", i2DArray[0][1]);
	printf("\n i2DArray[0][2] : %d", i2DArray[0][2]);

	printf("\n\n *** ROW 2 ***");
	printf("\n i2DArray[1][0] : %d", i2DArray[1][0]);
	printf("\n i2DArray[1][1] : %d", i2DArray[1][1]);
	printf("\n i2DArray[1][2] : %d", i2DArray[1][2]);

	printf("\n\n *** ROW 3 ***");
	printf("\n i2DArray[2][0] : %d", i2DArray[2][0]);
	printf("\n i2DArray[2][1] : %d", i2DArray[2][1]);
	printf("\n i2DArray[2][2] : %d", i2DArray[2][2]);

	printf("\n\n *** ROW 4 ***");
	printf("\n i2DArray[3][0] : %d", i2DArray[3][0]);
	printf("\n i2DArray[3][1] : %d", i2DArray[3][1]);
	printf("\n i2DArray[3][2] : %d", i2DArray[3][2]);

	printf("\n\n *** ROW 5 ***");
	printf("\n i2DArray[4][0] : %d", i2DArray[4][0]);
	printf("\n i2DArray[4][1] : %d", i2DArray[4][1]);
	printf("\n i2DArray[4][2] : %d", i2DArray[4][2]);

	printf("\n\n");
	return 0;
}