// To demonstrate variable arguments - sum of variable arguments - Method 2
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
	// Function  declaration
	int va_Add(int, va_list);

	// Variable declaration
	int total = 0;
	int number;
	va_list numberList;

	// Code
	va_start(numberList, n);
	total = va_Add(n, numberList);
	va_end(numberList);
	return total;
}

int va_Add(int n, va_list list) {
	// Variable declaration
	int temp_total = 0;

	// Code
	while(n) {
		temp_total += va_arg(list, int);
		n--;
	}
	return temp_total;
}