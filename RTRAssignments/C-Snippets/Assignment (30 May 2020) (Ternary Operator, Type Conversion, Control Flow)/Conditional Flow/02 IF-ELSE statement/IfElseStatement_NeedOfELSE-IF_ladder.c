// To demonstrate IF - ELSE statement - 3 (need of IF ELSE  ladder)
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int D;

	// Code 
	printf("\n Enter an integer, D : ");
	scanf("%d", &D);

	if(D < 0)		// IF - 01
		printf("\n %d is less than zero i.e. negative..", D);
	else {			// ELSE - 01
		if((D > 0) && (D <= 100))	// IF - 02
			printf("\n %d is between 0 and 100..", D);
		else {								// ELSE - 02
			if((D > 100) && (D <= 200))	// IF - 03
				printf("\n %d is between 100 and 200...", D);
			else {									// ELSE - 03
				if((D > 200) && (D <= 300))	// IF - 04
					printf("\n %d is between 200 and 300...", D);
				else {									// ELSE - 04
					if((D > 300) && (D <= 400))	// IF - 05
						printf("\n %d is between 300 and 400...", D);
					else {									// ELSE - 05
						if((D > 500) && (D <= 500))	// IF - 06
							printf("\n %d is between 400 and 500....", D);
						else
							printf("\n %d is greater than 500....",D);
					}
				}
			}
		}
	}

	printf("\n\n");
	return 0;
}