// To demonstrate ELSE IF ladder using example of month
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int month;

	// Code
	printf("\n Enter the number of month (1 - 12) : ");
	scanf("%d", &month);

	printf("\n Month number %d is ", month);
	if(month == 1)
		printf("JANUARY");
	else if(month == 2)
		printf("FEBRUARY");
	else if(month == 3)
		printf("MARCH");
	else if(month == 4)
		printf("APRIL");
	else if(month == 5)
		printf("MAY");
	else if(month == 6)
		printf("JUNE");
	else if(month == 7)
		printf("JULY");
	else if(month == 8)
		printf("AUGUST");
	else if(month == 9)
		printf("SEPTEMBER");
	else if(month == 10)
		printf("OCTOBER");
	else if(month == 11)
		printf("NOVEMBER");
	else if(month == 12)
		printf("DECEMBER");
	else
		printf("invalid...");
	printf("\n\n ELSE IF ladder completed...");

	printf("\n\n");
	return 0;
}