// To demonstrate WHILE loop - User input dependent
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, n, x;

	// Code
	printf("\n Enter an integer from where iteration must begin : ");
	scanf("%d", &x);
	printf("\n How many number you want to print after %d ", x);
	scanf("%d", &n);
	printf("\n Printing digits from %d to %d..", x, x + n);
	i = x;
	while(i <= (x + n)) {
		printf("\n %d", i);
		i++;
	}
	
	printf("\n\n");
	return 0;
}