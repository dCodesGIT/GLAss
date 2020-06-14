// To demonstrate structure initialization - Method 3 - single variable inline initialization
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>
int main() {
	// Local structure declaration
	struct MyData {
		int i;
		float f;
		char c;
		double d;
	}data = { 10,26.08f,'V',36.10 };		// Local structure variable

	// Code
	printf("\n DATA MEMBERS of 'struct MyData' are ");
	printf("\n i = %d", data.i);
	printf("\n f = %f", data.f);
	printf("\n d = %lf", data.d);
	printf("\n c = %c", data.c);

	printf("\n\n");
	return 0;
}