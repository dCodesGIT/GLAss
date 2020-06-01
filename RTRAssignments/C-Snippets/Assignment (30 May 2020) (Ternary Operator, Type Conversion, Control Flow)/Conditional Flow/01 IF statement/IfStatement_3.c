// To demonstrate IF statement - 3
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int n;

	// Code
	printf("\n Enter an integer : ");
	scanf("%d", &n);

	if(n < 0)
		printf("\n %d is less than zero i.e. negative.", n);
	if((n >= 0) && (n<=100))
		printf("\n %d is between 0 and 100", n);
	if((n > 100) && (n <= 200))
		printf("\n %d is between 100 and 200", n);
	if((n > 200) && (n <= 300))
		printf("\n %d is between 200 and 300", n);
	if((n > 300) && (n <= 400))
		printf("\n %d is between 300 and 400", n);
	if((n > 400) && (n <= 500))
		printf("\n %d is between 400 and 500", n);
	if(n > 500)
		printf("\n %d is greater than 500", n);

	printf("\n\n");
	return 0;
}