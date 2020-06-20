// To demonstrate union - size of union
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
struct MyStruct {
	int i;
	float f;
	double d;
	char c;
};
union MyUnion {
	int i;
	float f;
	double d;
	char c;
};

int main() {
	// Variable declaration
	struct MyStruct s;
	union MyUnion u;

	// Code
	printf("\n Size of MyStruct is %d", sizeof(s));
	printf("\n Size of MyUnion is %d", sizeof(u));

	printf("\n\n");
	return 0;
}