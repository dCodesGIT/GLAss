// To demonstrate ELSE IF ladder using Mathematical operations 
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
#include <conio.h>
int main() {
	// Variable declaration
	int a, b, result;
	char option, div_opt;

	// Code
	printf("\n Enter a number, A : ");
	scanf("%d", &a);
	printf("\n Enter another number, B : ");
	scanf("%d", &b);

	printf("\n Press \n '+' for Addition.");
	printf("\n '-' for Subtration.");
	printf("\n '*' for Multiplication.");
	printf("\n '/' for Division.");
	printf("\n Enter your choice : ");
	option = getche();
	printf("\n\n");
	if(option == '+') {
		result = a + b;
		printf("\n Addition of A = %d and B = %d gives %d", a, b, result);
	}
	else if(option == '-') {
		if(a > b) {
			result = a - b;
			printf("\n Subtration of B = %d from A = %d gives %d", b, a, result);
		}
		else {
			result = b - a;
			printf("\n Subtration of A = %d from B = %d gives %d", a, b, result);
		}
	}
	else if(option == '*') {
		result = a * b;
		printf("\n Multiplication of A = %d and B = %d gives %d", a, b, result);
	}
	else if(option == '/') {
		printf("\n\n Press\n 'Q' or 'q' or '/' for Quotient.");
		printf("\n 'R' or 'r' or '%' for Remainder.");
		printf("\n Enter your choice : ");
		div_opt = getche();
		if(div_opt == '/') {
			if(a > b) {
				result = a / b;
				printf("\n Division of A = %d by B = %d gives quotient %d", a, b, result);
			}
			else {
				result = b / a;
				printf("\n Division of B = %d by A = %d gives quotient %d", b, a, result);
			}
		}
		else if(div_opt == '%') {
			if(a > b) {
				result = a % b;
				printf("\n Division of A = %d by B = %d gives remainder %d", a, b, result);
			}
			else {
				result = b % a;
				printf("\n Division of B = %d by A = %d gives remainder %d", b, a, result);
			}
		}
		else {
			printf("\n Invalid character entered!!! Pls try again");
		}
	}
	else
		printf("\n Invalid character entered!!! Pls try again");
	printf("\n ELSE IF ladder completed..");

	printf("\n\n");
	return 0;
}