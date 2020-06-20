// To demonstrate structure inside structure - array inside nested structure
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>

struct Number {
	int n;
	int n_table[10];
};
struct NumberTables {
	struct Number a, b, c;
};

int main() {
	// Variable declaration
	struct NumberTables table;
	int i;

	// Code
	table.a.n = 2;
	for(i = 0; i < 10; i++)
		table.a.n_table[i] = table.a.n * (i + 1);
	printf("\n Table of %d", table.a.n);
	for(i = 0; i < 10; i++)
		printf("\n %d x %d = %d", table.a.n, (i + 1), table.a.n_table[i]);

	table.b.n = 5;
	for(i = 0; i < 10; i++)
		table.b.n_table[i] = table.b.n * (i + 1);
	printf("\n\n Table of %d", table.b.n);
	for(i = 0; i < 10; i++)
		printf("\n %d x %d = %d", table.b.n, (i + 1), table.b.n_table[i]);

	table.c.n = 10;
	for(i = 0; i < 10; i++)
		table.c.n_table[i] = table.c.n * (i + 1);
	printf("\n\n Table of %d", table.c.n);
	for(i = 0; i < 10; i++)
		printf("\n %d x %d = %d", table.c.n, (i + 1), table.c.n_table[i]);

	printf("\n\n");
	return 0;
}