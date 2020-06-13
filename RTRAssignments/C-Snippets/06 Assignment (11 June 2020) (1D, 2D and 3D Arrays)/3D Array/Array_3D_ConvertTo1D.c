// To convert 3D arrays into 1D array
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>

#define height 5
#define width 3
#define depth 2

int main() {
	// Variable declaration
	int iArray_3D[height][width][depth] = { {{1,2},{3,4},{5,6}},{{2,4},{6,8},{10,12}},{{3,6},{9,12},{15,18}},
		{{4,8},{12,16},{20,24}},{{5,10},{15,20},{25,30}} };
	int iArray_1D[height * width * depth];
	int i, j, k;

	// Code
	printf("\n Elements in 3D array are");
	for(i = 0; i < height; i++) {
		printf("\n\n *** ROW %d ***", i + 1);
		for(j = 0; j < width; j++) {
			printf("\n\t *** COLUMN %d ***", j + 1);
			printf("\n");
			for(k = 0; k < depth; k++)
				printf("\t\t iArray_3D[%d][%d][%d] = %d", height, width, depth, iArray_3D[i][j][k]);
		}
	}
	// Converting 3D array to 1D array
	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
			for(k = 0; k < depth; k++)
				iArray_1D[(i * width * depth) + (j * depth) + k] = iArray_3D[i][j][k];
	printf("\n Elements in 1D array are ");
	for(i = 0; i < height * width * depth; i++)
		printf("\n iArray_1D[%d] = %d", i + 1, iArray_1D[i]);

	printf("\n\n");
	return 0;
}