// To demonstrate FOR loop - User defined infinite loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	char option, ch = '\0';

	// Code
	printf("\n This is a user defined infinite loop...");
	printf("\n Once the infinite loop begins,\n Press 'Q' to quit the infinite loop.");
	printf("\n Press 'Y' to begin your infinite loop : ");
	option = getch();
	if(option == 'y' || option == 'Y') {
		for(;;)	// Infinite loop
		{
			printf("\n Inside Infinite loop...");
			ch = getch();
			if(ch == 'q' || ch == 'Q')
				break;
		}
	}
	printf("\n Successfully exitted infinite loop...");

	printf("\n\n");
	return 0;
}