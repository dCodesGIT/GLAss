// To demonstrate 1D array - string operation - character replacement
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Funcion declaration
	int stringLength(char[]);
	void stringCopy(char[], char[]);

	// Variable declaration
	char str_original[STRING_LENGTH], str_modified[STRING_LENGTH];
	int length, i;

	// Code
	printf("\n Enter a string : ");
	gets_s(str_original, STRING_LENGTH);
	stringCopy(str_modified, str_original);
	length = stringLength(str_original);
	for(i = 0; i < length; i++) {
		switch(str_modified[i]) {
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'i':
			case 'I':
			case 'o':
			case 'O':
			case 'u':
			case 'U':
				str_modified[i] = '*';
				break;
			default :
				break;
		}
	}
	printf("\n The original string entered by you is : ");
	puts(str_original);
	printf("\n The replaced string is : ");
	puts(str_modified);

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

void stringCopy(char dest[], char src[]) {
	int i = 0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
}