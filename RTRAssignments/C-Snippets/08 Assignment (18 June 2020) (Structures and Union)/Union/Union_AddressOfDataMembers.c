// To compare addresses i.e. memory allocation of structures and unions
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
	float  f;
	double d;
	char c;
};

int main() {
	// Variable declaration
	struct MyStruct s;
	union MyUnion u;

	// Code
	printf("\n Data members of struct are ");
	s.i = 10;
	s.f = 10.02f;
	s.d = 2000.1002;
	s.c = 'D';
	printf("\n s.i = %d", s.i);
	printf("\n s.f = %f", s.f);
	printf("\n s.d = %lf", s.d);
	printf("\n s.c = %c", s.c);
	printf("\n Address of members of struct are");
	printf("\n s.i = %p", &s.i);
	printf("\n s.f = %p", &s.f);
	printf("\n s.d = %p", &s.d);
	printf("\n s.c = %p", &s.c);
	printf("\n MyStruct s = %p", &s);

	printf("\n\n Members of union are");
	u.i = 26;
	printf("\n u.i = %d", u.i);
	u.f = 26.08f;
	printf("\n u.f = %f", u.f);
	u.d = 2000.2608;
	printf("\n u.d = %lf", u.d);
	u.c = 'V';
	printf("\n u.c = %c", u.c);
	printf("\n Address of members of Union are");
	printf("\n u.i = %p", &u.i);
	printf("\n u.f = %p", &u.f);
	printf("\n u.d = %p", &u.d);
	printf("\n u.c = %p", &u.c);
	printf("\n MyUnion u = %p", &u);

	printf("\n\n");
	return 0;
}