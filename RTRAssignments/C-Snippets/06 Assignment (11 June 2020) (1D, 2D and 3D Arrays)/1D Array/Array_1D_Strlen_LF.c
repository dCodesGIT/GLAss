// To demonstrate 1D array - string length - Library function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>
#define STRING_LENGTH 512
int main() {
	// Variable declaration
	char str[STRING_LENGTH];
	int strLen = 0;

	// Code
	printf("\n Enter a string : ");
	gets_s(str, STRING_LENGTH);
	printf("\n The string entered by you is : ");
	puts(str);
	strLen = strlen(str);
	printf("\n Length of string is : %d characters", strLen);

	printf("\n\n");
	return 0;
}