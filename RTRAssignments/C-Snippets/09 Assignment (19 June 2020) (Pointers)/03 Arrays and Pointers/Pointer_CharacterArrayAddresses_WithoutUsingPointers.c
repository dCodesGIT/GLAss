// To demonstrate pointers - Addresses of character array without using pointers
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int i;
	char cArray[10];

	// Code
	for(i = 0; i < 10; i++)
		cArray[i] = (char)(i + 65);
	printf("\n Elements in character array and their addresses are");
	for(i = 0; i < 10; i++)
		printf("\n cArray[%d] = %c \t Address = %p", i, cArray[i], &cArray[i]);

	printf("\n\n");
	return 0;
}