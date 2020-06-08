// To demonstrate 2 dimensional array - PieceMeal initialization of array of string (character by character)
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define MAX_STRING_LENGTH 512

int main() {
	// Variable declaration
	char strArray[5][10];
	int char_size, strArray_size;
	int strArray_rows, strArray_cols, strArray_n;
	int i;

	// Code
	char_size = sizeof(char);
	strArray_size = sizeof(strArray);
	printf("\n Size of 2D character array is %d", strArray_size);
	strArray_rows = strArray_size / sizeof(strArray[0]);
	printf("\n Number of rows in 2D array are %d", strArray_rows);
	strArray_cols = sizeof(strArray[0]) / char_size;
	printf("\n Number of cols in 2D array are %d", strArray_cols);
	strArray_n = strArray_rows * strArray_cols;
	printf("\n Maximum elements (character) in 2D array are %d", strArray_n);

	strArray[0][0] = 'M';
	strArray[0][1] = 'y';
	strArray[0][2] = '\0';

	strArray[1][0] = 'n';
	strArray[1][1] = 'a';
	strArray[1][2] = 'm';
	strArray[1][3] = 'e';
	strArray[1][4] = '\0';

	strArray[2][0] = 'i';
	strArray[2][1] = 's';
	strArray[2][2] = '\0';

	strArray[3][0] = 'D';
	strArray[3][1] = 'a';
	strArray[3][2] = 'r';
	strArray[3][3] = 's';
	strArray[3][3] = 'h';
	strArray[3][4] = 'a';
	strArray[3][5] = 'n';
	strArray[3][6] = '\0';

	strArray[4][0] = 'V';
	strArray[4][1] = 'i';
	strArray[4][2] = 'k';
	strArray[4][3] = 'a';
	strArray[4][4] = 'm';
	strArray[4][5] = '\0';

	printf("\n\n Strings in 2D array are : \n");
	for(i = 0; i < strArray_rows; i++)
		printf(" %s", strArray[i]);

	printf("\n\n");
	return 0;
}