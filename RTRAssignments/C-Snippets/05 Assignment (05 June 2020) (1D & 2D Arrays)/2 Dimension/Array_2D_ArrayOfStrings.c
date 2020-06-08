// To demonstrate 2 dimensional array - Array of strings
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define MAX_STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLenth(char[]);

	// Variable declaration
	char strArray[10][15] = { "Hello!!!","Welcome","to","Real","Time","Rendering","Batch","2020-21","of","AMC" };
	int char_size, strArray_size;
	int strArray_n, strArray_cols, strArray_rows;
	int actual_chars = 0, i;

	// Code
	char_size = sizeof(char);
	strArray_size = sizeof(strArray);
	printf("\n Size of 2D character array i.e. array of strings : %d",strArray_size);
	strArray_rows = strArray_size / sizeof(strArray[0]);
	printf("\n Number of rows in 2D charater array are %d", strArray_rows);
	strArray_cols = sizeof(strArray[0]) / char_size;
	printf("\n Number of columns in 2D charater array are %d", strArray_cols);
	strArray_n = strArray_cols * strArray_rows;
	printf("\n Maximum number of elements (characters) in 2D array are %d", strArray_n);
	for(i = 0; i < strArray_rows; i++)
		actual_chars = actual_chars + stringLength(strArray[i]);
	printf("\n Actual number of elements (charaters) in 2D array are %d", actual_chars);

	printf("\n\n Strings in 2D array are : \n");
	printf("%s ", strArray[0]);
	printf("%s ", strArray[1]);
	printf("%s ", strArray[2]);
	printf("%s ", strArray[3]);
	printf("%s ", strArray[4]);
	printf("%s ", strArray[5]);
	printf("%s ", strArray[6]);
	printf("%s ", strArray[7]);
	printf("%s ", strArray[8]);
	printf("%s ", strArray[9]);

	printf("\n\n");
	return 0;
}

// Function declaration
int stringLength(char string[]) {
	int j;
	for(j = 0; j < MAX_STRING_LENGTH; j++) {
		if(string[j] == '\0')
			return j;
	}
	return j;
}