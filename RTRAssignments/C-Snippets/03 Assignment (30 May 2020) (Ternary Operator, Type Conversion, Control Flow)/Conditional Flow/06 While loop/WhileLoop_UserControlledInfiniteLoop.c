// To demonstrate WHILE loop - User controlled infinite loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	char option, ch = '\0';

	// Code
	printf("\n This is a User controlled infinite loop..");
	printf("\n Once the infinite loop begins, \n Press 'q' to quit the infinite loop.");
	printf("\n Now press 'Y' to begin your infinte loop : ");
	option = getche();
	printf("\n");
	if(option == 'y' || option == 'Y') {
		while(1) {
			printf("\n Inside Infinite loop");
			ch = getch();
			if(ch == 'q' || ch == 'Q')
				break;
		}
		printf("\n\n Exitted your infinite loop successfully...");
	}
	else {
		printf("\n You must enter 'y' or 'Y' to initiate user controlled infinite loop...");
		printf("\n Pls try again..");
	}

	printf("\n\n");
	return 0;
}