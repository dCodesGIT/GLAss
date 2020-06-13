// To demonstrate 1D array - string concatenation - User defined function
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	void stringConcatenation(char[], char[]);

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

	stringConcatenation(str_1, str_2);
	printf("\n\n After Concatenation...");
	printf("\n The string 'str_1' entered by you is : ");
	puts(str_1);
	printf("\n The string 'str_2' entered by you is : ");
	puts(str_2);

	printf("\n\n");
	return 0;
}

// Function definition
void stringConcatenation(char string1[], char string2[]) {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	int string1_len, string2_len, i, j;

	// Code
	string1_len = stringLength(string1);
	string2_len = stringLength(string2);
	for(i = string1_len, j = 0; j < string2_len; i++, j++)
		string1[i] = string2[j];
	string1[i] = '\0';
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