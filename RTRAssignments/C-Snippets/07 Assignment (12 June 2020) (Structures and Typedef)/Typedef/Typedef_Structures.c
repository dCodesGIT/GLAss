// To demonstrate typedef on structures
// Date : 12 June 2020
// By : Darshan Vikam

#include<stdio.h>
#define MAX_LENGTH 100

struct Employee {
	char name[MAX_LENGTH];
	unsigned int age;
	char gender;
	double salary;
};

struct Data {
	int i;
	float f;
	double d;
	char c;
};

int main() {
	// Typedefs of structures
	typedef struct Employee MY_EMPLOYEE_TYPE;
	typedef struct Data MY_DATA_TYPE;

	// Variable declaration
	struct Employee emp = { "Funny",10,'M',500 };
	MY_EMPLOYEE_TYPE emp_typedef = { "Bunny",26,'M',25000 };
	struct Data d = { 10,26.08f,2000.0826,'V' };
	MY_DATA_TYPE d_typedef;

	// Code
	d_typedef.i = 26;
	d_typedef.f = 10.02f;
	d_typedef.d = 2000.0210;
	d_typedef.c = 'D';

	printf("\n Struct Employee : ");
	printf("\n\t emp.name = %s", emp.name);
	printf("\n\t emp.age = %d", emp.age);
	printf("\n\t emp.gender = %c", emp.gender);
	printf("\n\t emp.salary = %lf", emp.salary);

	printf("\n\n MY_EMPLOYEE_TYPE : ");
	printf("\n\t emp_typedef.name = %s", emp_typedef.name);
	printf("\n\t emp_typedef.age = %d", emp_typedef.age);
	printf("\n\t emp_typedef.gender = %c", emp_typedef.gender);
	printf("\n\t emp_typedef.salary = %lf", emp_typedef.salary);

	printf("\n\n struct Data : ");
	printf("\n\t d.i = %d", d.i);
	printf("\n\t d.f = %f", d.f);
	printf("\n\t d.d = %lf", d.d);
	printf("\n\t d.c = %c", d.c);

	printf("\n\n MY_DATA_TYPE : ");
	printf("\n\t d_typedef.i = %d", d_typedef.i);
	printf("\n\t d_typedef.f = %f", d_typedef.f);
	printf("\n\t d_typedef.d = %lf", d_typedef.d);
	printf("\n\t d_typedef.c = %c", d_typedef.c);

	printf("\n\n");
	return 0;
}