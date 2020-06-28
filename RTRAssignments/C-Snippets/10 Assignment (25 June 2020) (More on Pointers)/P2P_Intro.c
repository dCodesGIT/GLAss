// To demonstrate pointer to pointer - application of pointer to pointer
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int num;
	int *ptr = NULL, **pptr = NULL;

	// Code
	num = 26;
	printf("\n\n *** BEFORE prt = &num ***");
	printf("\n Value of 'num' = %d", num);
	printf("\n Address of 'num' = %p", &num);
	printf("\n Value at address of 'num' = %d", *(&num));

	ptr = &num;
	printf("\n\n *** AFTER prt = &num ***");
	printf("\n Value of 'num' = %d", num);
	printf("\n Address of 'num' = %p", ptr);
	printf("\n Value at address of 'num' = %d", *ptr);

	pptr = &ptr;
	printf("\n\n *** AFTER pprt = &ptr ***");
	printf("\n Value of 'num' = %d", num);
	printf("\n Address of 'num' (ptr) = %p", ptr);
	printf("\n Address of 'ptr' (pptr) = %p", pptr);
	printf("\n Value at address of 'num' (*ptr) = %d", *ptr);
	printf("\n Value at address of 'num' (**pptr) = %d", **pptr);
	printf("\n Value at address of 'ptr' (*pptr) = %p", *pptr);

	printf("\n\n");
	return 0;
}