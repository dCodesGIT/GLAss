// To demonstrate pointers - charater pointer declaration method 2
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	char ch;
	char* ptr = NULL;

	// Code
	ch = 'D';
	printf("\n *** Before ptr = &ch ***");
	printf("\n Value of 'ch' = %c", ch);
	printf("\n Adress of 'ch' = %p", &ch);
	printf("\n Value at address of 'ch' = %c", *(&ch));

	ptr = &ch;
	printf("\n\n *** After ptr = &ch ***");
	printf("\n Value of 'ch' = %c", ch);
	printf("\n Adress of 'ch' = %p", ptr);
	printf("\n Value at address of 'ch' = %c", *ptr);

	printf("\n\n");
	return 0;
}