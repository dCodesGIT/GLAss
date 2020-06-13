// To demonstrate 1D array - string operation - Remove spaces
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	char str[STRING_LENGTH], str_woSpaces[STRING_LENGTH];
	int length, i, j;

	// Code
	printf("\n Enter a string : ");
	gets_s(str, STRING_LENGTH);

	length = stringLength(str);
	for(i = 0, j = 0; i < length; i++)
		if(str[i] == ' ')
			continue;
		else
			str_woSpaces[j++] = str[i];
	str_woSpaces[j] = '\0';

	printf("\n The string entered by you is : %s", str);
	printf("\n\n The string without spaces is : %s", str_woSpaces);

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