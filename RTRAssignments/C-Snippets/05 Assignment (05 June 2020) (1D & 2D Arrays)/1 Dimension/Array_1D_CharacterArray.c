// To demonstrate 1 dimensional array - Character array
// Date : 05 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	char chArr1[] = { 'A','s','t','r','o','M','e','d','i','C','o','m','p','\0' };
	char chArr2[] = { 'W','e','l','c','o','m','e','s','\0' };
	char chArr3[] = { 'Y','o','u','\0' };
	char chArr4[] = "To";
	char chArr5[] = "RTR batch 2020-21";
	char chArr_WithoutNullTerminatingChar[] = { 'H','e','l','l' };

	// Code
	printf("\n Size of chArr1[] : %lu", sizeof(chArr1));
	printf("\n Size of chArr2[] : %lu", sizeof(chArr2));
	printf("\n Size of chArr3[] : %lu", sizeof(chArr3));
	printf("\n Size of chArr4[] : %lu", sizeof(chArr4));
	printf("\n Size of chArr5[] : %lu", sizeof(chArr5));

	printf("\n\n The strings are...");
	printf("\n chArr1[] : %s", chArr1);
	printf("\n chArr2[] : %s", chArr2);
	printf("\n chArr3[] : %s", chArr3);
	printf("\n chArr4[] : %s", chArr4);
	printf("\n chArr5[] : %s", chArr5);

	printf("\n Size of chArr_WithoutNullTerminatingChar[] : %lu", sizeof(chArr_WithoutNullTerminatingChar));
	printf("\n chArr_WithoutNullTerminatingChar[] : %s", chArr_WithoutNullTerminatingChar);

	printf("\n\n");
	return 0;
}