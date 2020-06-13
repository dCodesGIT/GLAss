// To demonstrate 1D array - String operation to capitalize all characters - Library function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<ctype.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	char str_original[STRING_LENGTH], str_capital[STRING_LENGTH];
	int length, i, j;

	// Code
	printf("\n Enter a string : ");
	gets_s(str_original,STRING_LENGTH);

	length = stringLength(str_original);
	for(i = 0; i < length; i++) {
		if(i == 0)
			str_capital[i] = toupper(str_original[i]);
		else if(str_original[i] == ' ') {
			str_capital[i] = str_original[i];
			i++;
			str_capital[i] = toupper(str_original[i]);
		}
		else
			str_capital[i] = str_original[i];
	}
	str_capital[i] = '\0';

	printf("\n The string entered by you is : %s", str_original);
	printf("\n The string with first letter of each word capital is\n");
	puts(str_capital);

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