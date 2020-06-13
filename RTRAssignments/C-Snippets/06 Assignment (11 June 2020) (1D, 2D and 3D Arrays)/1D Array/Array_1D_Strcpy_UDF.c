// To demonstrate 1D array - string copy - User defined function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	void stringCopy(char[], char[]);

	// Variable declaration
	char str_original[STRING_LENGTH], str_copy[STRING_LENGTH];

	// Code
	printf("\n Enter a string : ");
	gets_s(str_original, STRING_LENGTH);
	stringCopy(str_copy, str_original);
	printf("\n The original string entered by you is : ");
	puts(str_original);
	printf("\n The copied string is : ");
	puts(str_copy);

	printf("\n\n");
	return 0;
}

// Function declaration
void stringCopy(char copy[], char original[]) {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	int i, length;

	// Code
	length = stringLength(original);
	for(i = 0; i < length; i++)
		copy[i] = original[i];
	copy[i] = '\0';
}

int stringLength(char string[]) {
	// Variable declaration
	int i;

	// Code
	for(i = 0; i < STRING_LENGTH; i++)
		if(string[i] == '\0')
			break;
	return i;
}