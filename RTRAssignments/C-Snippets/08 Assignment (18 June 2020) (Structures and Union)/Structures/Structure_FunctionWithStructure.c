// To demonstrate structure - functions and structures
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
struct MyStruct {
	int i;
	float f;
	double d;
	char c;
};

int main() {
	// Function declaration
	struct MyStruct AddStruct(struct MyStruct, struct MyStruct, struct MyStruct);

	// Variable declaration
	struct MyStruct d1, d2, d3, dresult;

	// Code
	printf("\n Data 1");
	printf("\n Enter an integer value : ");
	scanf("%d", &d1.i);
	printf("\n Enter a floating value : ");
	scanf("%f", &d1.f);
	printf("\n Enter a double value : ");
	scanf("%lf", &d1.d);
	printf("\n Enter a character : ");
	scanf(" %c", &d1.c);

	printf("\n\n Data 2");
	printf("\n Enter an integer value : ");
	scanf("%d", &d2.i);
	printf("\n Enter a floating value : ");
	scanf("%f", &d2.f);
	printf("\n Enter a double value : ");
	scanf("%lf", &d2.d);
	printf("\n Enter a character : ");
	scanf(" %c", &d2.c);

	printf("\n\n Data 3");
	printf("\n Enter an integer value : ");
	scanf("%d", &d3.i);
	printf("\n Enter a floating value : ");
	scanf("%f", &d3.f);
	printf("\n Enter a double value : ");
	scanf("%lf", &d3.d);
	printf("\n Enter a character : ");
	scanf(" %c", &d3.c);

	dresult = AddStruct(d1, d2, d3);
	printf("\n\n Sum of structure members is");
	printf("\n dresult.i = %d", dresult.i);
	printf("\n dresult.f = %f", dresult.f);
	printf("\n dresult.d = %lf", dresult.d);
	dresult.c = d1.c;
	printf("\n dresult.c (from d1.c) = %c", dresult.c);
	dresult.c = d2.c;
	printf("\n dresult.c (from d2.c) = %c", dresult.c);
	dresult.c = d3.c;
	printf("\n dresult.c (from d3.c) = %c", dresult.c);

	printf("\n\n");
	return 0;
}

// Function definition
struct MyStruct AddStruct(struct MyStruct one, struct MyStruct two, struct MyStruct three) {
	// Variable declaration
	struct MyStruct ans;

	// Code
	ans.i = one.i + two.i + three.i;
	ans.f = one.f + two.f + three.f;
	ans.d = one.d + two.d + three.d;
	return ans;
}