// To demonstrate pointers - pointers and functions - pointers as out parameters method 2
// Date : 25 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdlib.h>
int main() {
	// Function declaration
	void MathematicalOperation(int, int, int *, int *, int *, int *, int *);

	// Variable declaration
	int a, b;
	int *sum = NULL, *diff = NULL, *pro = NULL, *quo = NULL, *rem = NULL;

	// Code
	printf("\n Enter a number : ");
	scanf("%d", &a);
	printf("\n Enter another number : ");
	scanf("%d", &b);

	sum = (int *)malloc(sizeof(int));
	if(sum == NULL) {
		printf("\n Could not allocate memory for 'sum'");
		exit(0);
	}
	diff = (int *)malloc(sizeof(int));
	if(diff == NULL) {
		printf("\n Could not allocate memory for 'diff'");
		exit(0);
	}
	pro = (int *)malloc(sizeof(int));
	if(pro == NULL) {
		printf("\n Could not allocate memory for 'pro'");
		exit(0);
	}
	quo = (int *)malloc(sizeof(int));
	if(quo == NULL) {
		printf("\n Could not allocate memory for 'quo'");
		exit(0);
	}
	rem = (int *)malloc(sizeof(int));
	if(rem == NULL) {
		printf("\n Could not allocate memory for 'rem'");
		exit(0);
	}
	printf("\n Memory allocated successfully for all the variables...");
	MathematicalOperation(a, b, sum, diff, pro, quo, rem);

	printf("\n *** Result ***");
	printf("\n Sum = %d", *sum);
	printf("\n Difference = %d", *diff);
	printf("\n Product = %d", *pro);
	printf("\n Quotient = %d", *quo);
	printf("\n Remainder = %d", *rem);

	if(sum) {
		free(sum);
		sum = NULL;
		printf("\n Memory for variable 'sum' freed !!!");
	}
	if(diff) {
		free(diff);
		diff = NULL;
		printf("\n Memory for variable 'diff' freed !!!");
	}
	if(pro) {
		free(pro);
		pro = NULL;
		printf("\n Memory for variable 'pro' freed !!!");
	}
	if(quo) {
		free(quo);
		quo = NULL;
		printf("\n Memory for variable 'quo' freed !!!");
	}
	if(rem) {
		free(rem);
		rem = NULL;
		printf("\n Memory for variable 'rem' freed !!!");
	}

	printf("\n\n");
	return 0;
}

// Function definition
void MathematicalOperation(int x, int y, int *add, int *sub, int *mult, int *quo, int *rem) {
	// Code
	*add = x + y;
	*sub = x - y;
	*mult = x * y;
	*quo = x / y;
	*rem = x % y;
}