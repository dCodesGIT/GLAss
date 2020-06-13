// To demonstrate 3D arrays - Inline initialization and piece meal display
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int iArray[5][3][2] = { {{1,2},{3,4},{5,6}},{{2,4},{6,8},{10,12}},{{3,6},{9,12},{15,18}},
		{{4,8},{12,16},{20,24}},{{5,10},{15,20},{25,30}} };
	int iArray_size, int_size;
	int height, width, depth, numOfEle;

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
	printf("\n\n *** ROW 1 ***");
	printf("\n\t *** COLUMN 1 ***");
	printf("\n\t\t iArray[0][0][0] = %d", iArray[0][0][0]);
	printf("\t\t iArray[0][0][1] = %d", iArray[0][0][1]);
	printf("\n\t *** COLUMN 2 ***");
	printf("\n\t\t iArray[0][1][0] = %d", iArray[0][1][0]);
	printf("\t\t iArray[0][1][1] = %d", iArray[0][1][1]);
	printf("\n\t *** COLUMN 3 ***");
	printf("\n\t\t iArray[0][2][0] = %d", iArray[0][2][0]);
	printf("\t\t iArray[0][2][1] = %d", iArray[0][2][1]);

	printf("\n\n *** ROW 2 ***");
	printf("\n\t *** COLUMN 1 ***");
	printf("\n\t\t iArray[1][0][0] = %d", iArray[1][0][0]);
	printf("\t\t iArray[1][0][1] = %d", iArray[1][0][1]);
	printf("\n\t *** COLUMN 2 ***");
	printf("\n\t\t iArray[1][1][0] = %d", iArray[1][1][0]);
	printf("\t\t iArray[1][1][1] = %d", iArray[1][1][1]);
	printf("\n\t *** COLUMN 3 ***");
	printf("\n\t\t iArray[1][2][0] = %d", iArray[1][2][0]);
	printf("\t\t iArray[1][2][1] = %d", iArray[1][2][1]);

	printf("\n\n *** ROW 3 ***");
	printf("\n\t *** COLUMN 1 ***");
	printf("\n\t\t iArray[2][0][0] = %d", iArray[2][0][0]);
	printf("\t\t iArray[2][0][1] = %d", iArray[2][0][1]);
	printf("\n\t *** COLUMN 2 ***");
	printf("\n\t\t iArray[2][1][0] = %d", iArray[2][1][0]);
	printf("\t\t iArray[2][1][1] = %d", iArray[2][1][1]);
	printf("\n\t *** COLUMN 3 ***");
	printf("\n\t\t iArray[2][2][0] = %d", iArray[2][2][0]);
	printf("\t\t iArray[2][2][1] = %d", iArray[2][2][1]);

	printf("\n\n *** ROW 4 ***");
	printf("\n\t *** COLUMN 1 ***");
	printf("\n\t\t iArray[3][0][0] = %d", iArray[3][0][0]);
	printf("\t\t iArray[3][0][1] = %d", iArray[3][0][1]);
	printf("\n\t *** COLUMN 2 ***");
	printf("\n\t\t iArray[3][1][0] = %d", iArray[3][1][0]);
	printf("\t\t iArray[3][1][1] = %d", iArray[3][1][1]);
	printf("\n\t *** COLUMN 3 ***");
	printf("\n\t\t iArray[3][2][0] = %d", iArray[3][2][0]);
	printf("\t\t iArray[3][2][1] = %d", iArray[3][2][1]);

	printf("\n\n *** ROW 5 ***");
	printf("\n\t *** COLUMN 1 ***");
	printf("\n\t\t iArray[4][0][0] = %d", iArray[4][0][0]);
	printf("\t\t iArray[4][0][1] = %d", iArray[4][0][1]);
	printf("\n\t *** COLUMN 2 ***");
	printf("\n\t\t iArray[4][1][0] = %d", iArray[4][1][0]);
	printf("\t\t iArray[4][1][1] = %d", iArray[4][1][1]);
	printf("\n\t *** COLUMN 3 ***");
	printf("\n\t\t iArray[4][2][0] = %d", iArray[4][2][0]);
	printf("\t\t iArray[4][2][1] = %d", iArray[4][2][1]);

	printf("\n\n");
	return 0;
}