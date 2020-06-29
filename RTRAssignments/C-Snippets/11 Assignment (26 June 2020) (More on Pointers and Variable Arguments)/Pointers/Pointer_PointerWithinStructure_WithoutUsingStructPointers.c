// To demonstrate pointers - pointers within structure - without using struct pointer
// Date : 26 June 2020
// By : Darshan Vikam

#include<stdio.h>

struct MyData {
	int i;
	int *iptr;
	float f;
	float *fptr;
	double d;
	double *dptr;
};

int main() {
	// Variable declaration
	struct MyData data;

	// Code
	data.i = 26;
	data.iptr = &data.i;
	data.f = 26.08;
	data.fptr = &data.f;
	data.d = 2000.2608;
	data.dptr = &data.d;

	printf("\n\n i = %d", *data.iptr);
	printf("\n Address of 'i' = %p", data.iptr);
	printf("\n\n f = %f", *data.fptr);
	printf("\n Address of 'f' = %p", data.fptr);
	printf("\n\n d = %lf", *data.dptr);
	printf("\n Address of 'd' = %p", data.dptr);

	printf("\n\n");
	return 0;
}