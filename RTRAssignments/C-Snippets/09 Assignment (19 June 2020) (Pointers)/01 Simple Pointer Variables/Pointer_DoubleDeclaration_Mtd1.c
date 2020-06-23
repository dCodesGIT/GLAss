// To demonstrate pointers - double pointer declaration method 1
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	double num;
	double *ptr = NULL;

	// Code
	num = 2000.1002;
	printf("\n *** Before ptr = &num ***");
	printf("\n Value of 'num' = %lf", num);
	printf("\n Adress of 'num' = %p", &num);
	printf("\n Value at address of 'num' = %lf", *(&num));

	ptr = &num;
	printf("\n\n *** After ptr = &num ***");
	printf("\n Value of 'num' = %lf", num);
	printf("\n Adress of 'num' = %p", ptr);
	printf("\n Value at address of 'num' = %lf", *ptr);

	printf("\n\n");
	return 0;
}