// To demonstrate 3D arrays - Inline initialization and loop display
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int iArray[5][3][2] = { {{1,2},{3,4},{5,6}},{{2,4},{6,8},{10,12}},{{3,6},{9,12},{15,18}},
		{{4,8},{12,16},{20,24}},{{5,10},{15,20},{25,30}} };
	int iArray_size, int_size;
	int height, width, depth, numOfEle;
	int i, j, k;

	// Code
	int_size = sizeof(int);
	iArray_size = sizeof(iArray);
	printf("\n Size of 3D array is : %d", iArray_size);
	width = iArray_size / sizeof(iArray[0]);
	printf("\n Number of rows (width) in 3D array is : %d", width);
	height = sizeof(iArray[0]) / sizeof(iArray[0][0]);
	printf("\n Number of columns (height) in 3D array is : %d", height);
	depth = sizeof(iArray[0][0]) / sizeof(iArray[0][0][0]);
	printf("\n Number of depth elements in 3D array is : %d", depth);
	numOfEle = height * width * depth;
	printf("\n Maximum number of elements in 3D array is : %d", numOfEle);

	printf("\n Elements in 3D array are ");
	for(i = 0; i < width; i++) {
		printf("\n\n *** ROW %d ***", i + 1);
		for(j = 0; j < height; j++) {
			printf("\n\t *** COLUMN %d ***\n", j + 1);
			for(k = 0; k < depth; k++)
				printf("\t\t iArray[%d][%d][%d] = %d", i, j, k, iArray[i][j][k]);
		}
	}

	printf("\n\n");
	return 0;
}