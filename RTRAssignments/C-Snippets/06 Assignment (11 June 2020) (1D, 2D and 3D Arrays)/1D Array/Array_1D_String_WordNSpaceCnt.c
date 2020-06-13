// To demonstrate 1D array - string operation - Word and Space count
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	char str[STRING_LENGTH];
	int length, i;
	int words, spaces = 0;
	
	// Code
	printf("\n Enter a string : ");
	gets_s(str, STRING_LENGTH);
	
	length = stringLength(str);
	for(i = 0; i < length; i++)
		if(str[i] == ' ')
			spaces++;
	words = spaces + 1;
	printf("\n The string entered by you is : %s", str);
	printf("\n Number of spaces in your string are %d", spaces);
	printf("\n Number of words in your string are %d", words);

	printf("\n\n");
	return 0;
}

// Function definition
int stringLength(char string[]) {
	int i;
	for(i = 0; i < STRING_LENGTH; i++)
		if(string[i] == '\0')
			break;
	return i;
}