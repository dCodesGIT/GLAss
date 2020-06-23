// To demonstrate pointers - integer pointer declaration method 2
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int num;
	int* ptr = NULL;

	// Code
	num = 10;
	printf("\n *** Before ptr = &num ***");
	printf("\n Value of 'num' = %d", num);
	printf("\n Adress of 'num' = %p", &num);
	printf("\n Value at address of 'num' = %d", *(&num));

	ptr = &num;
	printf("\n\n *** After ptr = &num ***");
	printf("\n Value of 'num' = %d", num);
	printf("\n Adress of 'num' = %p", ptr);
	printf("\n Value at address of 'num' = %d", *ptr);

	printf("\n\n");
	return 0;
}