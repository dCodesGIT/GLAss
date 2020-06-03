// To demonstrate IF - ELSE statement - 1
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int a, b, d;

	// Code
	a = 10;
	b = 26;
	d = 26;

	// First If- Else pair
	if(a < b) {
		printf("\n\n Entering 1st IF block...");
		printf("\n %d is less than %d", a, b);
	}
	else {
		printf("\n\n Entering 1st ELSE block....");
		printf("\n %d is not less than %d", a, b);
	}
	printf("\n First IF-ELSE block completed !!!!");

	// Second If-Else pair
	if(b != d) {
		printf("\n\n Entering 2nd IF block...");
		printf("\n %d is not equal to %d", a, d);
	}
	else {
		printf("\n\n Entering 2nd ELSE block....");
		printf("\n %d is equal to %d", a, d);
	}
	printf("\n Second IF-ELSE block completed !!!!");

	printf("\n\n");
	return 0;
}