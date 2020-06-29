// To demonstrate pointers - function pointers
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Function declaration
	int AddInts(int, int);
	int SubInts(int, int);
	float AddFloats(float, float);

	// Variable declaration
	typedef int (*IntPointerFunction)(int, int);
	IntPointerFunction ptr_AddInts = NULL;
	IntPointerFunction ptr_SubInts = NULL;
	typedef float (*FloatPointerFunction)(float, float);
	FloatPointerFunction ptr_AddFloat = NULL;
	int iResult = 0;
	float fResult = 0.0f;

	// Code
	ptr_AddInts = AddInts;
	iResult = ptr_AddInts(10, 26);
	printf("\n Sum of integers is : %d", iResult);

	ptr_SubInts = SubInts;
	iResult = ptr_SubInts(26, 10);
	printf("\n Difference of integers is : %d", iResult);

	ptr_AddFloat = AddFloats;
	fResult = ptr_AddFloat(10.02f, 26.08f);
	printf("\n Sum of floats is : %f", fResult);

	printf("\n\n");
	return 0;
}

// Function definition
int AddInts(int a, int b) {
	return (a + b);
}

int SubInts(int a, int b) {
	if(a > b)
		return (a - b);
	else
		return (b - a);
}

float AddFloats(float x, float y) {
	return (x + y);
}