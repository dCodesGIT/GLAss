// To demonstrate 1D array - string concatenation - Library function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>
#define STRING_LENGTH 512
int main() {
	// Variable declaration
	char str_1[STRING_LENGTH], str_2[STRING_LENGTH];

	// Code
	printf("\n Enter a string : ");
	gets_s(str_1, STRING_LENGTH);
	printf("\n Enetr another string : ");
	gets_s(str_2, STRING_LENGTH);

	printf("\n Before Concatenation...");
	printf("\n The string 'str_1' entered by you is : ");
	puts(str_1);
	printf("\n The string 'str_2' entered by you is : ");
	puts(str_2);

	strcat(str_1, str_2);
	printf("\n\n After Concatenation...");
	printf("\n The string 'str_1' entered by you is : ");
	puts(str_1);
	printf("\n The string 'str_2' entered by you is : ");
	puts(str_2);

	printf("\n\n");
	return 0;
}