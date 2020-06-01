// To demonstrate IF statement - 5
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int num;

	// Code
	num = 10;
	if(num)		// Non zero +ve value
		printf("\n If block - 1 : NUM exist and has value %d", num);
	num = -10;
	if(num)		// Non zero -ve value
		printf("\n If block - 2 : NUM exist and has value %d", num);
	num = 0;
	if(num)		// Zero valued
		printf("\n If block - 3 : NUM exist and has value %d", num);
	printf("\n All the three statements are done....");

	printf("\n\n");
	return 0;
}