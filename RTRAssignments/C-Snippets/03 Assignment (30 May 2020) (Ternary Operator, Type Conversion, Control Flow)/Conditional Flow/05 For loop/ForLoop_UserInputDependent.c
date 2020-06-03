// To demonstrate FOR loop - User input dependent
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int n, x, i;

	// Code
	printf("\n Enter an integer to from where iteration should begin : ");
	scanf("%d", &x);
	printf("\n How many numbers you want to print from %d : ", x);
	scanf("%d", &n);

	for(i = x; i <= (x + n); i++)
		printf("\n %d", i);

	printf("\n\n");
	return 0;
}