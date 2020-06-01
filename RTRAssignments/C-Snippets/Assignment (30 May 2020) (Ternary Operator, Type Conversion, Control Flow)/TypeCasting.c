// To demonstrate Type Casting
// Date : 30 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// Variable declaration
	int i, j;
	char ch_1, ch_2;
	int a, int_result;
	float f, float_result;
	int i_explicit;
	float f_explicit;

	// Code
	// Implicit type casting between 'int' and 'char'
	i = 68;
	ch_1 = i;
	printf("\n\n Integer I = %d", i);
	printf("\n Type casting integer I = %d to character ch_1 = %c", i, ch_1);

	ch_2 = 'D';
	j = ch_2;
	printf("\n\n Character ch_2 = %c", ch_2);
	printf("\n Type casting charater ch_2 = %c to integer J = %d", ch_2, j);

	// Implicit converstion of 'int' to 'float'
	a = 10;
	f = 26.08f;
	float_result = a + f;
	printf("\n\n Type casting integer 'A' to float 'F'.");
	printf("\n Sum of integer, A = %d and float, F = %f gives the result in float", a, f);
	printf("\n Result = %f", float_result);

	int_result = a + f;
	printf("\n\n Type casting float 'F' to integer 'A'.");
	printf("\n Sum of integer, A = %d and float, F = %f gives the result in integer on type casting", a, f);
	printf("\n Result = %d", int_result);

	// Explicit type casting using CAST operator
	f_explicit = 26.081002f;
	i_explicit = (int)f_explicit;
	printf("\n\n Floating value is : %f", f_explicit);
	printf("\n On type casting explicitally to integer, it gives");
	printf("\n Integer value is : %d", i_explicit);

	printf("\n\n");
	return 0;
}