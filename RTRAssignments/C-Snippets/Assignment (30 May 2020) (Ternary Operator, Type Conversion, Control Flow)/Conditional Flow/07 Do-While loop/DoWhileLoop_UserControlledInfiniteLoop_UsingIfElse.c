// To demonstrate DO WHILE loop - User controlled infinite loop using if else
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	char option, ch = '\0';

	// Code
	printf("\n This is an infinite loop.");
	printf("\n Once the infinite loop begins, \n press 'q' to quit the infinite loop.");
	printf("\n Enter 'Y' to initiate the user controlled infinite loop : ");
	option = getche();
	if(option == 'y' || option == 'Y') {
		do {
			printf("\n Inside infinite loop....");
			ch = getch();
			if(ch == 'Q' || ch == 'q')
				break;
		} while(1);
		printf("\n Exitting infinite loop successfully...");
	}
	else {
		printf("\n You must press 'y' or 'Y' to initiate user controlled infinite loop..");
		printf("\n Pls try again....");
	}
	printf("\n\n");
	return 0;
}