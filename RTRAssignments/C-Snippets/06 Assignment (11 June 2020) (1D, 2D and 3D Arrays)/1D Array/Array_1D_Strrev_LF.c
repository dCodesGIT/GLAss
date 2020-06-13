// To demonstrate 1D array - string reverse - Library function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>
#define STRING_LENGTH 512
int main() {
	// Variable declaration
	char str[STRING_LENGTH];

	// Code
	printf("\n Enter a string : ");
	gets_s(str, STRING_LENGTH);
	printf("\n The string entered by you is : ");
	puts(str);
	printf("\n The reversed string is : %s", strrev(str));

	printf("\n\n");
	return 0;
}