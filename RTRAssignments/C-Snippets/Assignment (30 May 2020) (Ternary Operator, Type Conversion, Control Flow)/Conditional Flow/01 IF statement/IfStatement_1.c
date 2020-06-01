// To demonstrate IF statement - 1
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int a, b, c;

	// Code
	printf("\n Enter 3 integers with spaces in between.\n");
	scanf("%d %d %d", &a, &b, &c);
	if(a < b)
		printf("\n %d is LESS than %d", a, b);
	if(b != c)
		printf("\n %d is NOT EQUAL to %d", b, c);
	
	printf("\n\n");
	return 0;
}