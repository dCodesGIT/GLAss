// To demonstrate pointer - Copy of pointer
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int num;
	int *ptr = NULL, *ptr_copy = NULL;

	// Code
	num = 26;
	ptr = &num;
	printf("\n *** Before ptr_copy = ptr ***");
	printf("\n num = %d", num);
	printf("\n &num = %p", &num);
	printf("\n *(&num) = %d", *(&num));
	printf("\n ptr = %p", ptr);
	printf("\n *ptr = %d", *ptr);
	
	ptr_copy = ptr;
	printf("\n\n *** After ptr_copy = ptr ***");
	printf("\n num = %d", num);
	printf("\n &num = %p", &num);
	printf("\n *(&num) = %d", *(&num));
	printf("\n ptr = %p", ptr);
	printf("\n *ptr = %d", *ptr);
	printf("\n ptr_copy = %p", ptr_copy);
	printf("\n *ptr_copy = %d", *ptr_copy);

	printf("\n\n");
	return 0;
}