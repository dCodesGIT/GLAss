// To demonstrate DO WHILE loop - User controlled infinite loop without using if else
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	char option, ch = '\0';

	// Code
	printf("\n This is user controlled Infinite loop...");
	printf("\n Once the infinite loop begins, \n press 'q' to quit the infinite loop...");
	do {
		do {
			printf("\n Inside infinite loop....");
			ch = getch();
		} while(ch != 'q' && ch != 'Q');
		printf("\n Exitting user control infinite loop..");
		printf("\n Do you want to begin infinite loop again (Y/N) : ");
		option = getche();
	} while(option == 'y' || option == 'Y');

	printf("\n\n");
	return 0;
}