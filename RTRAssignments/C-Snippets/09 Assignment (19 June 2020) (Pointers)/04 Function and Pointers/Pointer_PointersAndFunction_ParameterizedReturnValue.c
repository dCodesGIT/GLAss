// To demonstrate pointers - pointers and functions - Parameterized return value
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>

enum {
	NEGATIVE = -1,
	ZERO,
	POSITIVE
};

int main() {
	// Function declaration
	int Difference(int, int, int *);

	// Variable declaration
	int a, b;
	int ans, ret;

	// Code
	printf("\n Enter a number, A : ");
	scanf("%d", &a);
	printf("\n Enter another number, B : ");
	scanf("%d", &b);

	ret = Difference(a, b, &ans);
	printf("\n The difference of %d and %d is %d", a, b, ans);

	if(ret == NEGATIVE)
		printf("\n %d is NEGATIVE", ans);
	else if(ret == POSITIVE)
		printf("\n %d is POSITIVE", ans);
	else
		printf("\n %d is ZERO", ans);

	printf("\n\n");
	return 0;
}

// Function definition
int Difference(int x, int y, int *result) {
	*result = x - y;
	if(*result > 0)
		return POSITIVE;
	else if(*result < 0)
		return NEGATIVE;
	else
		return ZERO;
}