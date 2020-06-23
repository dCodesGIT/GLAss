// To demonstrate pointers - Addresses of character array using pointers
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int i;
	char cArray[10];
	char *cArray_ptr = NULL;

	// Code
	for(i = 0; i < 10; i++)
		cArray[i] = (char)(i + 65);
	cArray_ptr = cArray;
	printf("\n Elements in character array and their addresses are");
	for(i = 0; i < 10; i++)
		printf("\n cArray[%d] = %c \t Address = %p", i, *(cArray_ptr + i), (cArray_ptr + i));

	printf("\n\n");
	return 0;
}