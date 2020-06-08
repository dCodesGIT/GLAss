// To demonstrate 2 dimensional array - PieceMeal initialization of array of string (string by string)
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define MAX_STRING_LENGTH 512

int main() {
	// Function declaration
	void stringCopy(char[], char[]);

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
	
	stringCopy(strArray[0], "My");
	stringCopy(strArray[1], "name");
	stringCopy(strArray[2], "is");
	stringCopy(strArray[3], "Darshan");
	stringCopy(strArray[4], "Vikam");

	printf("\n Strings in 2D array are : ");
	for(i = 0; i < strArray_rows; i++)
		printf(" %s", strArray[i]);

	printf("\n\n");
	return 0;
}

// Function declaration
void stringCopy(char dest[], char src[]) {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	int i = 0, len;

	// Code
	len = stringLength(src);
	for(i = 0; i < len; i++)
		dest[i] = src[i];
	dest[i] = '\0';
}

int stringLength(char string[]) {
	int j;
	for(j = 0; j < MAX_STRING_LENGTH; j++) {
		if(string[j] == '\0')
			return j;
	}
	return j;
}