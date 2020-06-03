// To demonstrate user defined funtion - call method 1
// Date : 01 June 2020
// By : Darshan Vikam

#include <stdio.h>
int main(int argc, char *argv[], char *envp[]) {
	// Function declaration
	void addIt(void);
	int subIt(void);
	void proIt(int, int);
	int divIt(int, int);

	// Variable declaration
	int result;
	int a, b;

	// Code
	addIt();

	result = subIt();
	printf("\n Subtraction result is : %d", result);

	printf("\n\n Enter an integer : ");
	scanf("%d", &a);
	printf("\n Enter another integer : ");
	scanf("%d", &b);
	proIt(a, b);

	printf("\n\n Enter an integer : ");
	scanf("%d", &a);
	printf("\n Enter another integer : ");
	scanf("%d", &b);
	result = divIt(a, b);
	printf("\n Division results in : %d (quotient)", result);

	printf("\n\n");
	return 0;
}

// Function declaration
void addIt(void) {
	// Variable declaration
	int x, y, sum;
	
	printf("\n\n Enter an integer : ");
	scanf("%d", &x);
	printf("\n Enter another integer : ");
	scanf("%d", &y);
	sum = x + y;
	printf("\n Sum result is : %d", sum);
}

int subIt(void) {
	// Variable declaration
	int x, y;

	printf("\n\n Enter an integer : ");
	scanf("%d", &x);
	printf("\n Enter another integer : ");
	scanf("%d", &y);
	return (x - y);
}

void proIt(int x, int y) {
	printf("\n Product result is : %d", x * y);
}

int divIt(int x, int y) {
	if(x > y)
		return(x / y);
	else
		return(y / x);
}