// To demonstrate 1D array - string reverse - User defined function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	void stringReverse(char[], char[]);

	// Variable declaration
	char str_original[STRING_LENGTH], str_reverse[STRING_LENGTH];

	// Code
	printf("\n Enter a string : ");
	gets_s(str_original, STRING_LENGTH);
	printf("\n The orginal string entered by you is : ");
	puts(str_original);
	stringReverse(str_reverse, str_original);
	printf("\n The reversed string is : %s", str_reverse);

	printf("\n\n");
	return 0;
}

// Function declaration
void stringReverse(char reverse[], char original[]) {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	int i, length;

	// Code
	length = stringLength(original);
	length--;
	for(i = 0; i <= length; i++)
		reverse[i] = original[length - i];
	reverse[i] = '\0';
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