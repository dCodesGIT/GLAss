// To demonstrate a structure - Method 2 - single variable
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
struct MyData data;		// Global structure variable declaration

int main() {
	// Local Variable declaration
	int i_size, f_size, c_size, d_size;
	int struct_MyData_size;

	// Code
	data.i = 10;
	data.f = 26.08f;
	data.c = 'V';
	data.d = 36.10;

	printf("\n DATA MEMBERS of 'struct MyData' are ");
	printf("\n i = %d", data.i);
	printf("\n f = %f", data.f);
	printf("\n d = %lf", data.d);
	printf("\n c = %c", data.c);

	printf("\n\n Sizes (in Bytes) of data members of 'struct MyData' are");
	i_size = sizeof(data.i);
	f_size = sizeof(data.f);
	d_size = sizeof(data.d);
	c_size = sizeof(data.c);
	printf("\n Size of 'i' is : %d bytes", i_size);
	printf("\n Size of 'f' is : %d bytes", f_size);
	printf("\n Size of 'd' is : %d bytes", d_size);
	printf("\n Size of 'c' is : %d bytes", c_size);

	struct_MyData_size = sizeof(struct MyData);
	printf("\n\n Size of 'struct MyData' is : %d", struct_MyData_size);

	printf("\n\n");
	return 0;
}