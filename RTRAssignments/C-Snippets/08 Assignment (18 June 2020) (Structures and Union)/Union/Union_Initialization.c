// To demonstrate union - Initializing union
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>
union MyUnion {
	int i;
	float f;
	double d;
	char c;
};

int main() {
	// Variable declaration
	union MyUnion u1, u2;

	// Code
	u1.i = 10;
	u1.f = 10.02f;
	u1.d = 2000.0210;
	u1.c = 'D';
	printf("\n Members of union u1 are");
	printf("\n u1.i = %d", u1.i);
	printf("\n u1.f = %f", u1.f);
	printf("\n u1.d = %lf", u1.d);
	printf("\n u1.c = %c", u1.c);
	printf("\n Address of members of Union u1 are");
	printf("\n u1.i = %p", &u1.i);
	printf("\n u1.f = %p", &u1.f);
	printf("\n u1.d = %p", &u1.d);
	printf("\n u1.c = %p", &u1.c);
	printf("\n MyUnion u1 = %p", &u1);

	printf("\n\n Members of union u2 are");
	u2.i = 26;
	printf("\n u2.i = %d", u2.i);
	u2.f = 26.08f;
	printf("\n u2.f = %f", u2.f);
	u2.d = 2000.2608;
	printf("\n u2.d = %lf", u2.d);
	u2.c = 'V';
	printf("\n u2.c = %c", u2.c);
	printf("\n Address of members of Union u2 are");
	printf("\n u2.i = %p", &u2.i);
	printf("\n u2.f = %p", &u2.f);
	printf("\n u2.d = %p", &u2.d);
	printf("\n u2.c = %p", &u2.c);
	printf("\n MyUnion u2 = %p", &u2);

	printf("\n\n");
	return 0;
}