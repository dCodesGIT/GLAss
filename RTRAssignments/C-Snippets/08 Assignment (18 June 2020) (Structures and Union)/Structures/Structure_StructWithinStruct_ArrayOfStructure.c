// To demonstrate structure inside structure - array of nested structure
// Date : 18 June 2020
// By : Darshan Vikam

#include<stdio.h>

struct Number {
	int n;
	int n_table[10];
};
struct NumberTables {
	struct Number num;
};

int main() {
	// Variable declaration
	struct NumberTables tables[10];
	int i, j;

	// Code
	for(i = 0; i < 10; i++)
		tables[i].num.n = i + 1;
	for(i = 0; i < 10; i++) {
		printf("\n\n Table of %d", i + 1);
		for(j = 0; j < 10; j++) {
			tables[i].num.n_table[j] = tables[i].num.n * (j + 1);
			printf("\n %d x %d = %d", tables[i].num.n, j + 1, tables[i].num.n_table[j]);
		}
	}
	printf("\n\n");
	return 0;
}