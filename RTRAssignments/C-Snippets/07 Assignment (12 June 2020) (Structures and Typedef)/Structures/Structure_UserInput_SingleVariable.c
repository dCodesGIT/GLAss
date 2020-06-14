// To demonstrate structure - User input single variable
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
	// Local structure variable declaration
	struct MyData data;

	// Code
	printf("\n Enter Integer value for data member 'i' of 'struct MyData' : ");
	scanf("%d", &data.i);
	printf("\n Enter Float value for data member 'f' of 'struct MyData' : ");
	scanf("%f", &data.f);
	printf("\n Enter Character value for data member 'c' of 'struct MyData' : ");
	scanf(" %c", &data.c);
	printf("\n Enter Double value for data member 'd' of 'struct MyData' : ");
	scanf("%lf", &data.d);

	printf("\n DATA MEMBERS of 'struct MyData' are ");
	printf("\n i = %d", data.i);
	printf("\n f = %f", data.f);
	printf("\n c = %c", data.c);
	printf("\n d = %lf", data.d);

	printf("\n\n");
	return 0;
}