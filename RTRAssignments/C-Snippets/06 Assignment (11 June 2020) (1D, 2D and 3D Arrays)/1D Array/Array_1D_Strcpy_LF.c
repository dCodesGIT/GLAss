// To demonstrate 1D array - string copy - Library function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>
#define STRING_LENGTH 512
int main() {
	// Variable declaration
	char str_original[STRING_LENGTH], str_copy[STRING_LENGTH];

	// Code
	printf("\n Enter a string : ");
	gets_s(str_original, STRING_LENGTH);
	strcpy(str_copy, str_original);
	printf("\n The original string entered by you is : ");
	puts(str_original);
	printf("\n The copied string is : ");
	puts(str_copy);

	printf("\n\n");
	return 0;
}