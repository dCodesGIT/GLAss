// To demonstrate BREAK statement - using one simple loop
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
#include <conio.h>
int main() {
	// Variable decalration
	char ch;
	int i;

	// Code
	printf("\n Printing even number from 1 to 100 for every user input.");
	printf("\n Exiting the loop when user enter 'q' or 'Q'.");
	printf("\n\n");
	for(i = 0; i <= 100; i+=2) {
		printf("\t %d", i);
		ch = getch();
		if(ch == 'q' || ch == 'Q')
			break;
	}
	printf("\n Exiting loop.");

	printf("\n\n");
	return 0;
}