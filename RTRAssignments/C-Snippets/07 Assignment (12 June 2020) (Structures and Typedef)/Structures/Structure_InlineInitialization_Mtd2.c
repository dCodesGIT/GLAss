// To demonstrate structure initialization - Method 2 - single variable inline initialization
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>

// Global structure declaration
struct MyData {
	int i;
	float f;
	char c;
	double d;
};
struct MyData data = { 10,26.08f,'V',36.10 };		// Global structure variable

int main() {
	// Code
	printf("\n DATA MEMBERS of 'struct MyData' are ");
	printf("\n i = %d", data.i);
	printf("\n f = %f", data.f);
	printf("\n d = %lf", data.d);
	printf("\n c = %c", data.c);

	printf("\n\n");
	return 0;
}