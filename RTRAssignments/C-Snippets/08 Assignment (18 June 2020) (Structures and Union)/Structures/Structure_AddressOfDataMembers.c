// To demonstrate structure - Address of data members of structure
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
struct Mydata {
	int i;
	float f;
	double d;
	char c;
};

int main() {
	// Variable declaration
	struct Mydata data;

	// Code
	data.i = 10;
	data.f = 10.2f;
	data.d = 2000.1002;
	data.c = 'D';

	printf("\n Data members of 'struct Mydata' are");
	printf("\n i = %d", data.i);
	printf("\n f = %f", data.f);
	printf("\n d = %lf", data.d);
	printf("\n c = %c", data.c);

	printf("\n\n Addresses of data members of 'struct Mydata' are");
	printf("\n 'i' occupies address from %p", &data.i);
	printf("\n 'f' occupies address from %p", &data.f);
	printf("\n 'd' occupies address from %p", &data.d);
	printf("\n 'c' occupies address from %p", &data.c);

	printf("\n\n Starting address of 'struct Mydata' is %p", &data);

	printf("\n\n");
	return 0;
}