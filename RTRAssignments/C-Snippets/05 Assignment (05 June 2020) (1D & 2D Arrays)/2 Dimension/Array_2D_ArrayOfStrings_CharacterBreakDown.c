// To demonstrate 2 dimensional array - Array of strings break down
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
#define MAX_STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	char strArray[10][15] = { "Hello!!!","Welcome","to","Real","Time","Rendering","Batch","2020-21","of","AMC" };
	int strArray_len[10];
	int strArray_size, strArray_rows;
	int i, j;

	// Code
	strArray_size = sizeof(strArray);
	strArray_rows = strArray_size / sizeof(strArray[0]);
	for(i = 0; i < strArray_rows; i++)
		strArray_len[i] = stringLength(strArray[i]);
	printf("\n The entire string array is : \n");
	for(i = 0; i < strArray_rows; i++)
		printf(" %s", strArray[i]);

	printf("\n\n Strings in 2D array are : ");
	for(i = 0; i < strArray_rows; i++) {
		printf("\n String number %d => %s", i + 1, strArray[i]);
		for(j = 0; j < strArray_len[i]; j++)
			printf("\n Character %d - %c", j + 1, strArray[i][j]);
		printf("\n");
	}

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