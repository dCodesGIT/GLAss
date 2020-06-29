// To demonstrate variable arguments - number search - Method 2
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
#include<stdarg.h>

#define NUM 26
#define NUM_OF_ELEMENTS 10

int main() {
	// Function declaration
	void NumberSearch(int, int, ...);

	// Code
	printf("\n Number to search is %d", NUM);
	NumberSearch(NUM, NUM_OF_ELEMENTS, 16, 26, 54, 85, 35, 15, 26, 75, 95, 10, 25);

	printf("\n\n");
	return 0;
}

// Function defintion
void NumberSearch(int srchElement, int size, ...) {
	// Function declaration
	int va_NumberSearch(int, int, va_list);

	// Variable declaration
	int count = 0;
	va_list numList;

	// Code
	va_start(numList, size);
	count = va_NumberSearch(srchElement, size, numList);
	if(count == 0)
		printf("\n Number %d not found", srchElement);
	else
		printf("\n Number %d found %d times !!!", srchElement, count);
	va_end(numList);
}

int va_NumberSearch(int srch, int n, va_list list) {
	// Variable declaration
	int cnt = 0;

	// Code
	while(n) {
		if(va_arg(list, int) == srch)
			cnt++;
		n--;
	}
	return cnt;
}