// To demonstrate conversion of 2D array to 1D array
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>

#define rows 5
#define cols 3

int main() {
	// Variable decalration
	int iArray_2D[rows][cols], iArray_1D[rows * cols];
	int i, j, num;

	// Code
	printf("\n Enter %d elements to fill up 2D array", rows * cols);
	for(i = 0; i < rows; i++) {
		printf("\n *** Row %d ***\n", i + 1);
		for(j = 0; j < cols; j++) {
			printf(" iArray_2d[%d][%d] = ", i, j);
			scanf("%d", &iArray_2D[i][j]);
		}
	}

	// Converting 2D array to 1D array
	for(i = 0; i < rows; i++)
		for(j = 0; j < cols; j++)
			iArray_1D[(i * cols) + j] = iArray_2D[i][j];

	printf("\n Converted 1D array is");
	for(i = 0; i < rows * cols; i++)
		printf("\n iArray_1D[%d] = %d", i, iArray_1D[i]);

	printf("\n\n");
	return 0;
}