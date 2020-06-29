// To demonstrate variable arguments - sum of variable arguments - Method 1
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdarg.h>

int main() {
	// Function declaration
	int Add(int, ...);

	// Variable declaration
	int sum;

	// Code
	sum = Add(5, 10, 20, 30, 40, 50);
	printf("\n Sum = %d", sum);
	sum = Add(10, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
	printf("\n Sum = %d", sum);
	sum = Add(0);
	printf("\n Sum = %d", sum);

	printf("\n\n");
	return 0;
}

// Function definition
int Add(int n, ...) {				// Variadic function
	// Variable declaration
	int total = 0;
	int number;
	va_list numberList;

	// Code
	va_start(numberList, n);
	while(n) {
		number = va_arg(numberList, int);
		total += number;
		n--;
	}
	va_end(numberList);
	return total;
}