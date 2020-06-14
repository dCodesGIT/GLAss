// To demonstrate structure initialization - Method 4 - single variable inline initialization
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

int main() {
	// Local structure variable
	struct MyData data_1 = { 10,26.08f,'V',36.10 };
	struct MyData data_2 = { 'D',10.02f,26,36.10 };
	struct MyData data_3 = { 10,'P'};
	struct MyData data_4 = { 75};

	// Code
	printf("\n DATA MEMBERS of 'struct MyData data_1' are ");
	printf("\n i = %d", data_1.i);
	printf("\n f = %f", data_1.f);
	printf("\n d = %lf", data_1.d);
	printf("\n c = %c", data_1.c);

	printf("\n DATA MEMBERS of 'struct MyData data_2' are ");
	printf("\n i = %d", data_2.i);
	printf("\n f = %f", data_2.f);
	printf("\n d = %lf", data_2.d);
	printf("\n c = %c", data_2.c);

	printf("\n DATA MEMBERS of 'struct MyData data_3' are ");
	printf("\n i = %d", data_3.i);
	printf("\n f = %f", data_3.f);
	printf("\n d = %lf", data_3.d);
	printf("\n c = %c", data_3.c);

	printf("\n DATA MEMBERS of 'struct MyData data_4' are ");
	printf("\n i = %d", data_4.i);
	printf("\n f = %f", data_4.f);
	printf("\n d = %lf", data_4.d);
	printf("\n c = %c", data_4.c);

	printf("\n\n");
	return 0;
}