// To demonstrate 1D array - string length - User defined function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	char str[STRING_LENGTH];
	int strLen;

	// Code
	printf("\n Enter a string : ");
	gets_s(str, STRING_LENGTH);
	printf("\n The string entered by you is : ");
	puts(str);
	strLen = stringLength(str);
	printf("\n Length of string is : %d charaters", strLen);

	printf("\n\n");
	return 0;
}

// Function definition
int stringLength(char string[]) {
	// Variable declaration
	int i;
	
	// Code
	for(i = 0; i < STRING_LENGTH; i++) {
		if(string[i] == '\0')
			break;
	}
	return i;
}
