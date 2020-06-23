// To demonstrate pointer - Addresses of double arrays without using pointers
// Date : 19 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Variable declaration
	int i;
	double dArray[10];

	// Code
	for(i = 0; i < 10; i++)
		dArray[i] = (double)(i + 1) * 1.3333;
	printf("\n Elements in double array and thier addresses are");
	for(i = 0; i < 10; i++)
		printf("\n dArray[%d] = %lf \t Address = %p", i, dArray[i], &dArray[i]);

	printf("\n\n");
	return 0;
}