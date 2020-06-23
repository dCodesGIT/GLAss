// To demonstrate pointers - float pointer declaration method 2
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	float num;
	float* ptr = NULL;

	// Code
	num = 10.02f;
	printf("\n *** Before ptr = &num ***");
	printf("\n Value of 'num' = %f", num);
	printf("\n Adress of 'num' = %p", &num);
	printf("\n Value at address of 'num' = %f", *(&num));

	ptr = &num;
	printf("\n\n *** After ptr = &num ***");
	printf("\n Value of 'num' = %f", num);
	printf("\n Adress of 'num' = %p", ptr);
	printf("\n Value at address of 'num' = %f", *ptr);

	printf("\n\n");
	return 0;
}