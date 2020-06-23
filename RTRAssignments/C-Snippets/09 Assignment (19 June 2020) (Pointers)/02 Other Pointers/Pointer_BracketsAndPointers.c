// To demonstrate pointer - Backets and pointer operations
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int num, ans;
	int *ptr;

	// Code
	num = 26;
	ptr = &num;
	printf("\n num = %d", num);
	printf("\n &num = %p", &num);
	printf("\n *(&num) = %d", *(&num));
	printf("\n ptr = %p", ptr);
	printf("\n *ptr = %d", *ptr);

	printf("\n Result of (ptr + 10) = %p", (ptr + 10));
	printf("\n Result of *(ptr + 10) = %d", *(ptr + 10));
	printf("\n Result of (*ptr + 10) = %d", (*ptr + 10));
	++ptr;
	printf("\n Result of ++*ptr = %d", *ptr);
	*ptr++;
	printf("\n Result of *ptr++ = %d", *ptr);
	ptr = &num;
	(*ptr)++;
	printf("\n Result of (*ptr)++ = %d", *ptr);

	printf("\n\n");
	return 0;
}