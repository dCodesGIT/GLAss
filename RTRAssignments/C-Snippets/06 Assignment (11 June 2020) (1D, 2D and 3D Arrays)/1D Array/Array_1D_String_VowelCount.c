// To demonstrate 1D array - string operation - vowel count
// Date : 11 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define STRING_LENGTH 512
int main() {
	// Function declaration
	int stringLength(char[]);

	// Variable declaration
	char str[STRING_LENGTH];
	int length, j;
	int a = 0, e = 0, i = 0, o = 0, u = 0;

	// Code
	printf("\n Enter a string : ");
	gets_s(str, STRING_LENGTH);
	printf("\n The string entered by you is : ");
	puts(str);

	length = stringLength(str);
	for(j = 0; j < length; j++) {
		if(str[j] == 'a' || str[j] == 'A')
			a++;
		else if(str[j] == 'e' || str[j] == 'E')
			e++;
		else if(str[j] == 'i' || str[j] == 'I')
			i++;
		else if(str[j] == 'o' || str[j] == 'O')
			o++;
		else if(str[j] == 'u' || str[j] == 'U')
			u++;
	}
	printf("\n Occurance of vowels in your string is");
	printf("\n 'A' has occured %d times.", a);
	printf("\n 'E' has occured %d times.", e);
	printf("\n 'I' has occured %d times.", i);
	printf("\n 'O' has occured %d times.", o);
	printf("\n 'U' has occured %d times.", u);

	printf("\n\n");
	return 0;
}

// Function definition
int stringLength(char string[]) {
	// Variable declaration
	int i;

	// Code
	for(i = 0; i < STRING_LENGTH; i++)
		if(string[i] == '\0')
			break;
	return i;
}