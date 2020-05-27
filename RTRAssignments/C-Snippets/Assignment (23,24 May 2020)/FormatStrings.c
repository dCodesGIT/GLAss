// To view string formats

#include <stdio.h>
int main() {
	// Code
	printf("\n Hello World !!!");
	
	int a = 13;
	printf("\n\n Integer Decimal value of 'a' = %d", a);
	printf("\n Integer Octal value of 'a' = %o", a);
	printf("\n Integer Hexadecimal vlaue of 'a' (in lower case) = %x", a);
	printf("\n Integer Hexadecimal value of 'a' (in upper case) = %X", a);

	char ch = 'A';
	printf("\n\n Character ch = %c", ch);

	char str[] = "AstroMediComp's RTR batch 3.0 (2020 - 2021)";
	printf("\n\n String str = %s", str);

	long num = 30121995L;
	printf("\n\n Long integer num = %ld", num);

	unsigned int b = 7;
	printf("\n\n Unsigned integer b = %u", b);

	float f_num = 3012.1995f;
	printf("\n\n Floating point number with just %%f 'f_num' = %f", f_num);
	printf("\n Floating point number with %%4.2f 'f_num' = %4.2f", f_num);
	printf("\n Floating point number with %%6.5f 'f_num' = %6.5f", f_num);

	double d_pi = 3.141592542369751942983218;
	printf("\n\n Double precision floating point number without exponential = %g", d_pi);
	printf("\n Double precision floating point number with exponential (in lower case) = %e", d_pi);
	printf("\n Double precision floating point number with exponential (in upper case) = %E", d_pi);
	printf("\n Heamdecimal value of 'd_pi' (in lower case) = %a", d_pi);
	printf("\n Heamdecimal value of 'd_pi' (in upper case) = %A", d_pi);

	printf("\n\n");
	return 0;
}
