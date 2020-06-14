// To calculate array size - 1D, 2D and 3D
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int iArray_1D[5], iArray_2D[5][3], iArray_3D[100][100][5];
	int rows_2D, cols_2D, height_3D, width_3D, depth_3D;

	// Code
	printf("\n Size of 1D array 'iArray_1D' is : %d", sizeof(iArray_1D));
	printf("\n Number of elements in 1D arrar are : %d", sizeof(iArray_1D) / sizeof(iArray_1D[0]));

	printf("\n\n Size of 2D array 'iArray_2D' is : %d", sizeof(iArray_2D));
	rows_2D = sizeof(iArray_2D) / sizeof(iArray_2D[0]);
	printf("\n Number of rows in 2D array are : %d", rows_2D);
	cols_2D = sizeof(iArray_2D[0]) / sizeof(iArray_2D[0][0]);
	printf("\n Number of columns in 2D array are : %d", cols_2D);
	printf("\n Number of elements in 2D array are : %d", rows_2D * cols_2D);

	printf("\n\n Size of 3D array 'iArray_3D' is : %d", sizeof(iArray_3D));
	height_3D = sizeof(iArray_3D) / sizeof(iArray_3D[0]);
	printf("\n Height of 3D array is : %d", height_3D);
	width_3D = sizeof(iArray_3D[0]) / sizeof(iArray_3D[0][0]);
	printf("\n Width of 3D array is : %d", width_3D);
	depth_3D = sizeof(iArray_3D[0][0]) / sizeof(iArray_3D[0][0][0]);
	printf("\n Depth of 3D array is : %d", depth_3D);
	printf("\n Number of elements in 3D array are : %d", height_3D * width_3D * depth_3D);

	printf("\n\n");
	return 0;
}