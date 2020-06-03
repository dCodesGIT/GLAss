// To demonstrate Bitwise AND operators
// Date : 29 May 2020
// By : Darshan Vikam

#include <stdio.h>
int main() {
	// function declaration
	void InttoBin(unsigned int);
	// Variable declaration
	unsigned int D_a, D_b, D_result;

	// Code
	printf("\n Enter an integer, A : ");
	scanf("%u", &D_a);
	printf("\n Enter another integer, B : ");
	scanf("%u", &D_b);

	D_result = D_a & D_b;
	printf("\n\n Bitwise ANDing of A = %d (decimal) and B = %d (decimal) gives", D_a, D_b);
	printf("\n Result = %d",D_result);

	printf("\n\n Decimal Number \t Binary Equivalent");
	InttoBin(D_a);
	InttoBin(D_b);
	printf("\n");
	InttoBin(D_result);

	printf("\n\n");
	return 0;
}

// Function definition
void InttoBin(unsigned int dec) {
	// Variable declaration
	unsigned int quo, rem, num;
	unsigned int bin[8];
	int i;

	// Code
	for(i = 0; i < 8; i++)
		bin[i] = 0;
	num = dec;
	i = 7;
	while(num != 0) {
		quo = num / 2;
		rem = num % 2;
		bin[i--] = rem;
		num = quo;
	}
	printf("\n %d \t ===> \t ", dec);
	for(i = 0; i < 8; i++)
		printf(" %u", bin[i]);
}
