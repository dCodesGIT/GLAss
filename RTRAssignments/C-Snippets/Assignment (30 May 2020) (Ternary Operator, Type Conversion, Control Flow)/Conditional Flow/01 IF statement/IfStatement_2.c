// To demonstrate IF statement - 2
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int age;

	// Code
	printf("\n Enter age : ");
	scanf("%d", &age);
	if(age >= 18)
		printf("\n You are eligible for voting.");

	printf("\n\n");
	return 0;
}