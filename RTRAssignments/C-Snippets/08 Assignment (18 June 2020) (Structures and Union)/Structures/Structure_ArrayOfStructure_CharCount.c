// To demonstrate structure - Array of structure - character count
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<string.h>

#define STRING_LENGTH 1024

struct CharacterCount {
	char ch;
	int count;
}CharCounter[] = { {'A',0},{'B',0},{'C',0},{'D',0},{'E',0},{'F',0},{'G',0},{'H',0},{'I',0},{'J',0},{'K',0}, {'L',0},{'M',0},{'N',0},{'O',0},{'P',0},{'Q',0},{'R',0},{'S',0},{'T',0},{'U',0},{'V',0},{'W',0},{'X',0},{'Y',0},{'Z',0} };

#define SIZEOF_ARRAY_OF_STRUCT sizeof(CharCounter)
#define SIZEOF_SINGLE_STRUCT sizeof(CharCounter[0])
#define NUM_OF_ELEMENTS (SIZEOF_ARRAY_OF_STRUCT/SIZEOF_SINGLE_STRUCT)

int main() {
	// Variable declaration
	char string[STRING_LENGTH];
	int i, j, actual_string_length = 0;

	// Code
	printf("\n Enter a string : ");
	gets_s(string, STRING_LENGTH);
	actual_string_length = strlen(string);

	printf("\n The string you have entered is : \n");
	printf("%s", string);
	for(i = 0; i < actual_string_length; i++) {
		for(j = 0; j < NUM_OF_ELEMENTS; j++) {
			string[i] = toupper(string[i]);
			if(string[i] == CharCounter[j].ch)
				CharCounter[j].count++;
		}
	}
	printf("\n The number of occurences of all alphabets are as follows");
	printf("\n\n Character \t Occurances");
	for(i=0;i<NUM_OF_ELEMENTS;i++)
		printf("\n\t %c \t %d", CharCounter[i].ch, CharCounter[i].count);

	printf("\n\n");
	return 0;	
}