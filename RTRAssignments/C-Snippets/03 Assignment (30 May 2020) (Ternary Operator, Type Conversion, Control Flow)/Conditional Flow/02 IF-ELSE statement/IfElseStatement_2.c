// To demonstrate IF - ELSE statement - 2
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int age;

	// Code
	printf("\n Enter age : ");
	scanf("%d", &age);

	if(age >= 18) {
		printf("\n\n  Entered IF block...");
		printf("\n You are ELIGIBLE for voting !!!!");
	}
	else {
		printf("\n\n Entered ELSE block....");
		printf("\n You are NOT ELIGIBLE for voting !!!!");
	}
	printf("\n Bye..");

	printf("\n\n");
	return 0;
}