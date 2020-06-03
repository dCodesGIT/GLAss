// To demonstrate ELSE IF ladder using example of Character recogonization
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
#include <conio.h>

// ASCII values for 'A' to 'Z' ==> 65 to 90
#define UPPER_ALPHA_BEGIN 65
#define UPPER_ALPHA_END 90

// ASCII values for 'a' to 'z' ==> 97 to 122
#define LOWER_ALPHA_BEGIN 97
#define LOWER_ALPHA_END 122

// ASCII values for '0' to '9' ==> 48 to 57
#define ASCII_DIGIT_BEGIN 48
#define ASCII_DIGIT_END 57

int main() {
	// Variable declaration
	int ascii;
	char ch;

	// Code
	printf("\n Enter a character : ");
	ch = getch();

	if(ch == 'a' || ch == 'A' || ch == 'e' || ch == 'E' || ch == 'i' || ch == 'I' || ch == 'o' || ch == 'O' || ch == 'u' || ch == 'U')
		printf("\n Character \'%c\' you entered is a VOWEL.", ch);
	else {
			ascii = (int)ch;
			if((ascii >= UPPER_ALPHA_BEGIN && ascii <= UPPER_ALPHA_END) || (ascii >= LOWER_ALPHA_BEGIN && ascii <= LOWER_ALPHA_END))
				printf("\n Character \'%c\' you entered is a CONSONANT.", ch);
			else if(ascii >= ASCII_DIGIT_BEGIN && ascii <= ASCII_DIGIT_END)
				printf("\n Character \'%c\' you entered is a DIGIT", ch);
			else
				printf("\n Character \'%c\' you entered is a SPECIAL CHARACTER.", ch);
	}
	printf("\n ELSE IF ladder completed...");

	printf("\n\n");
	return 0;
}