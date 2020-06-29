// To demonstrate variable arguments - number search - Method 1
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
	NumberSearch(NUM, NUM_OF_ELEMENTS, 16, 26, 54, 85, 35, 15, 26, 75, 95, 10, 25);
	
	printf("\n\n");
	return 0;
}

// Function defintion
void NumberSearch(int srchElement, int size, ...) {
	// Variable declaration
	int count = 0;
	int n;
	va_list numList;

	// Code
	va_start(numList, size);
	while(size) {
		n = va_arg(numList, int);
		if(n == srchElement)
			count++;
		size--;
	}
	if(count == 0)
		printf("\n Number %d not found", srchElement);
	else
		printf("\n Number %d found %d times !!!", srchElement, count);
	va_end(numList);
}